/*
 * VISCA(tm) Camera Control Library
 * Copyright (C) 2002 Damien Douxchamps 
 *
 * Written by Damien Douxchamps <ddouxchamps@users.sf.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "libvisca.h"
#include <QDebug>

#ifdef VISCA_WIN
# ifdef _DEBUG
#  define DEBUG 1
# else
#  undef DEBUG
# endif
#endif

extern "C" {

/********************************/
/*      PRIVATE FUNCTIONS       */
/********************************/

void
_VISCA_append_byte(VISCAPacket_t *packet, unsigned char byte)
{
  packet->bytes[packet->length]=byte;
  (packet->length)++;
}


void
_VISCA_init_packet(VISCAPacket_t *packet)
{
  // we start writing at byte 1, the first byte will be filled by the
  // packet sending function. This function will also append a terminator.
  packet->length=1;
}


VISCA_API uint32_t
_VISCA_get_reply(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  // first message: -------------------
  if (_VISCA_get_packet(iface)!=VISCA_SUCCESS) 
    return VISCA_FAILURE;
  iface->type=iface->ibuf[1]&0xF0;

  // skip ack messages
  while (iface->type==VISCA_RESPONSE_ACK)
    {
      if (_VISCA_get_packet(iface)!=VISCA_SUCCESS) 
        return VISCA_FAILURE;
      iface->type=iface->ibuf[1]&0xF0;
    }
 
  switch (iface->type)
    {
    case VISCA_RESPONSE_CLEAR:
      return VISCA_SUCCESS;
      break;
    case VISCA_RESPONSE_ADDRESS:
      return VISCA_SUCCESS;
      break;
    case VISCA_RESPONSE_COMPLETED:
      return VISCA_SUCCESS;
      break;
    case VISCA_RESPONSE_ERROR:
      return VISCA_SUCCESS;
      break;
    }
  return VISCA_FAILURE;
}

VISCA_API uint32_t
_VISCA_send_packet_with_reply(VISCAInterface_t *iface, VISCACamera_t *camera, VISCAPacket_t *packet)
{
  if (_VISCA_send_packet(iface,camera,packet)!=VISCA_SUCCESS)
    return VISCA_FAILURE;

  if (_VISCA_get_reply(iface,camera)!=VISCA_SUCCESS)
    return VISCA_FAILURE;

  return VISCA_SUCCESS;    
}


/****************************************************************************/
/*                           PUBLIC FUNCTIONS                               */
/****************************************************************************/


/***********************************/
/*       SYSTEM  FUNCTIONS         */
/***********************************/

VISCA_API uint32_t
VISCA_set_address(VISCAInterface_t *iface, int *camera_num)
{
  VISCAPacket_t packet;
  int backup;
  VISCACamera_t camera; /* dummy camera struct */

  camera.address=0;
  backup=iface->broadcast;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet,0x30);
  _VISCA_append_byte(&packet,0x01);

  iface->broadcast=1;
  if (_VISCA_send_packet(iface, &camera, &packet)!=VISCA_SUCCESS)
    {
      iface->broadcast=backup;
      return VISCA_FAILURE;
    }
  else
    iface->broadcast=backup;
  
  if (_VISCA_get_reply(iface, &camera)!=VISCA_SUCCESS)
    return VISCA_FAILURE;
  else
    {
      /* We parse the message from the camera here  */
      /* We expect to receive 4*camera_num bytes,
         every packet should be 88 30 0x FF, x being
         the camera id+1. The number of cams will thus be
         ibuf[bytes-2]-1  */
      if ((iface->bytes & 0x3)!=0) /* check multiple of 4 */
	return VISCA_FAILURE;
      else
	{
	  *camera_num=iface->ibuf[iface->bytes-2]-1;
	  if ((*camera_num==0)||(*camera_num>7))
	    return VISCA_FAILURE;
	  else
	    return VISCA_SUCCESS;
	}
    }
  
}


VISCA_API uint32_t
VISCA_clear(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet,0x01);
  _VISCA_append_byte(&packet,0x00);
  _VISCA_append_byte(&packet,0x01);

  if (_VISCA_send_packet(iface, camera, &packet)!=VISCA_SUCCESS)
    return VISCA_FAILURE;
  else
    if (_VISCA_get_reply(iface, camera)!=VISCA_SUCCESS)
      return VISCA_FAILURE;
    else
      return VISCA_SUCCESS;
}

VISCA_API uint32_t
VISCA_get_camera_info(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;
  packet.bytes[0]=0x80 | camera->address;
  packet.bytes[1]=0x09;
  packet.bytes[2]=0x00;
  packet.bytes[3]=0x02;
  packet.bytes[4]=VISCA_TERMINATOR;
  packet.length=5;

  if (_VISCA_write_packet_data(iface, camera, &packet)!=VISCA_SUCCESS)
    return VISCA_FAILURE;
  else
    if (_VISCA_get_reply(iface, camera)!=VISCA_SUCCESS)
      return VISCA_FAILURE;

  if (iface->bytes!= 10) /* we expect 10 bytes as answer */
    return VISCA_FAILURE;
  else
    {
      camera->vendor=(iface->ibuf[2]<<8) + iface->ibuf[3];
      camera->model=(iface->ibuf[4]<<8) + iface->ibuf[5];
      camera->rom_version=(iface->ibuf[6]<<8) + iface->ibuf[7];
      camera->socket_num=iface->ibuf[8];
      return VISCA_SUCCESS;
    }
}

/***********************************/
/*       COMMAND FUNCTIONS         */
/***********************************/

VISCA_API uint32_t
VISCA_set_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_POWER);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_keylock(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_KEYLOCK);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_camera_id(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t id)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_ID);
  _VISCA_append_byte(&packet, (id & 0xF000) >> 12);
  _VISCA_append_byte(&packet, (id & 0x0F00) >>  8);
  _VISCA_append_byte(&packet, (id & 0x00F0) >>  4);
  _VISCA_append_byte(&packet, (id & 0x000F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_zoom_tele(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_ZOOM);
  _VISCA_append_byte(&packet, VISCA_ZOOM_TELE);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_zoom_wide(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_ZOOM);
  _VISCA_append_byte(&packet, VISCA_ZOOM_WIDE);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_zoom_stop(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_ZOOM);
  _VISCA_append_byte(&packet, VISCA_ZOOM_STOP);
  qDebug()<<"BA ZOOM STOP:"<<QByteArray((char*)packet.bytes, packet.length).toHex(' ');

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_zoom_tele_speed(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t speed)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_ZOOM);
  _VISCA_append_byte(&packet, VISCA_ZOOM_TELE_SPEED | (speed & 0x7));
  qDebug()<<"BA ZOOM TELE:"<<QByteArray((char*)packet.bytes, packet.length).toHex(' ');

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_zoom_wide_speed(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t speed)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_ZOOM);
  _VISCA_append_byte(&packet, VISCA_ZOOM_WIDE_SPEED | (speed & 0x7));
  qDebug()<<"BA ZOOM WIDE:"<<QByteArray((char*)packet.bytes, packet.length).toHex(' ');

  return _VISCA_send_packet_with_reply(iface, camera, &packet);

}


VISCA_API uint32_t
VISCA_set_zoom_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t zoom)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_ZOOM_VALUE);
  _VISCA_append_byte(&packet, (zoom & 0xF000) >> 12);
  _VISCA_append_byte(&packet, (zoom & 0x0F00) >>  8);
  _VISCA_append_byte(&packet, (zoom & 0x00F0) >>  4);
  _VISCA_append_byte(&packet, (zoom & 0x000F));
  qDebug()<<"BA ZOOM VALUE:"<<QByteArray((char*)packet.bytes, packet.length).toHex(' ');

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_zoom_and_focus_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t zoom, uint32_t focus)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_ZOOM_FOCUS_VALUE);
  _VISCA_append_byte(&packet, (zoom & 0xF000) >> 12);
  _VISCA_append_byte(&packet, (zoom & 0x0F00) >>  8);
  _VISCA_append_byte(&packet, (zoom & 0x00F0) >>  4);
  _VISCA_append_byte(&packet, (zoom & 0x000F));
  _VISCA_append_byte(&packet, (focus & 0xF000) >> 12);
  _VISCA_append_byte(&packet, (focus & 0x0F00) >>  8);
  _VISCA_append_byte(&packet, (focus & 0x00F0) >>  4);
  _VISCA_append_byte(&packet, (focus & 0x000F));
 
  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_dzoom(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_DZOOM);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_dzoom_limit(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t limit)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_DZOOM_LIMIT);
  _VISCA_append_byte(&packet, limit);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_dzoom_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_DZOOM_MODE);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_focus_far(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS);
  _VISCA_append_byte(&packet, VISCA_FOCUS_FAR);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_focus_near(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS);
  _VISCA_append_byte(&packet, VISCA_FOCUS_NEAR);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_focus_stop(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS);
  _VISCA_append_byte(&packet, VISCA_FOCUS_STOP);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_focus_far_speed(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t speed)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS);
  _VISCA_append_byte(&packet, VISCA_FOCUS_FAR_SPEED | (speed & 0x7));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_focus_near_speed(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t speed)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS);
  _VISCA_append_byte(&packet, VISCA_FOCUS_NEAR_SPEED | (speed & 0x7));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_focus_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t focus)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS_VALUE);
  _VISCA_append_byte(&packet, (focus & 0xF000) >> 12);
  _VISCA_append_byte(&packet, (focus & 0x0F00) >>  8);
  _VISCA_append_byte(&packet, (focus & 0x00F0) >>  4);
  _VISCA_append_byte(&packet, (focus & 0x000F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_focus_auto(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS_AUTO);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_focus_one_push(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS_ONE_PUSH);
  _VISCA_append_byte(&packet, VISCA_FOCUS_ONE_PUSH_TRIG);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_focus_infinity(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS_ONE_PUSH);
  _VISCA_append_byte(&packet, VISCA_FOCUS_ONE_PUSH_INF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_focus_autosense_high(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS_AUTO_SENSE);
  _VISCA_append_byte(&packet, VISCA_FOCUS_AUTO_SENSE_HIGH);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_focus_autosense_low(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS_AUTO_SENSE);
  _VISCA_append_byte(&packet, VISCA_FOCUS_AUTO_SENSE_LOW);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_focus_near_limit(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t limit)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS_NEAR_LIMIT);
  _VISCA_append_byte(&packet, (limit & 0xF000) >> 12);
  _VISCA_append_byte(&packet, (limit & 0x0F00) >>  8);
  _VISCA_append_byte(&packet, (limit & 0x00F0) >>  4);
  _VISCA_append_byte(&packet, (limit & 0x000F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_whitebal_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t mode)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_WB);
  _VISCA_append_byte(&packet, mode);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_whitebal_one_push(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_WB_TRIGGER);
  _VISCA_append_byte(&packet, VISCA_WB_ONE_PUSH_TRIG);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_rgain_up(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_RGAIN);
  _VISCA_append_byte(&packet, VISCA_UP);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_rgain_down(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_RGAIN);
  _VISCA_append_byte(&packet, VISCA_DOWN);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_rgain_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_RGAIN);
  _VISCA_append_byte(&packet, VISCA_RESET);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);

}


VISCA_API uint32_t
VISCA_set_rgain_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_RGAIN_VALUE);
  _VISCA_append_byte(&packet, (value & 0xF000) >> 12);
  _VISCA_append_byte(&packet, (value & 0x0F00) >>  8);
  _VISCA_append_byte(&packet, (value & 0x00F0) >>  4);
  _VISCA_append_byte(&packet, (value & 0x000F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_bgain_up(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_BGAIN);
  _VISCA_append_byte(&packet, VISCA_UP);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_bgain_down(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_BGAIN);
  _VISCA_append_byte(&packet, VISCA_DOWN);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_bgain_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_BGAIN);
  _VISCA_append_byte(&packet, VISCA_RESET);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_bgain_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_BGAIN_VALUE);
  _VISCA_append_byte(&packet, (value & 0xF000) >> 12);
  _VISCA_append_byte(&packet, (value & 0x0F00) >>  8);
  _VISCA_append_byte(&packet, (value & 0x00F0) >>  4);
 _VISCA_append_byte(&packet, (value & 0x000F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_shutter_up(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_SHUTTER);
  _VISCA_append_byte(&packet, VISCA_UP);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_shutter_down(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_SHUTTER);
  _VISCA_append_byte(&packet, VISCA_DOWN);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_shutter_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_SHUTTER);
  _VISCA_append_byte(&packet, VISCA_RESET);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_shutter_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_SHUTTER_VALUE);
  _VISCA_append_byte(&packet, (value & 0xF000) >> 12);
  _VISCA_append_byte(&packet, (value & 0x0F00) >>  8);
  _VISCA_append_byte(&packet, (value & 0x00F0) >>  4);
 _VISCA_append_byte(&packet, (value & 0x000F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_iris_up(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_IRIS);
  _VISCA_append_byte(&packet, VISCA_UP);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_iris_down(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_IRIS);
  _VISCA_append_byte(&packet, VISCA_DOWN);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_iris_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_IRIS);
  _VISCA_append_byte(&packet, VISCA_RESET);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_iris_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_IRIS_VALUE);
  _VISCA_append_byte(&packet, (value & 0xF000) >> 12);
  _VISCA_append_byte(&packet, (value & 0x0F00) >>  8);
  _VISCA_append_byte(&packet, (value & 0x00F0) >>  4);
 _VISCA_append_byte(&packet, (value & 0x000F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_gain_up(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_GAIN);
  _VISCA_append_byte(&packet, VISCA_UP);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_gain_down(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_GAIN);
  _VISCA_append_byte(&packet, VISCA_DOWN);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_gain_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_GAIN);
  _VISCA_append_byte(&packet, VISCA_RESET);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_gain_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_GAIN_VALUE);
  _VISCA_append_byte(&packet, (value & 0xF000) >> 12);
  _VISCA_append_byte(&packet, (value & 0x0F00) >>  8);
  _VISCA_append_byte(&packet, (value & 0x00F0) >>  4);
  _VISCA_append_byte(&packet, (value & 0x000F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_bright_up(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_BRIGHT);
  _VISCA_append_byte(&packet, VISCA_UP);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_bright_down(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_BRIGHT);
  _VISCA_append_byte(&packet, VISCA_DOWN);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_bright_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_BRIGHT);
  _VISCA_append_byte(&packet, VISCA_RESET);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_bright_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_BRIGHT_VALUE);
  _VISCA_append_byte(&packet, (value & 0xF000) >> 12);
  _VISCA_append_byte(&packet, (value & 0x0F00) >>  8);
  _VISCA_append_byte(&packet, (value & 0x00F0) >>  4);
  _VISCA_append_byte(&packet, (value & 0x000F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_aperture_up(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_APERTURE);
  _VISCA_append_byte(&packet, VISCA_UP);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_aperture_down(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_APERTURE);
  _VISCA_append_byte(&packet, VISCA_DOWN);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_aperture_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_APERTURE);
  _VISCA_append_byte(&packet, VISCA_RESET);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_aperture_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_APERTURE_VALUE);
  _VISCA_append_byte(&packet, (value & 0xF000) >> 12);
  _VISCA_append_byte(&packet, (value & 0x0F00) >>  8);
  _VISCA_append_byte(&packet, (value & 0x00F0) >>  4);
  _VISCA_append_byte(&packet, (value & 0x000F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_exp_comp_up(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_EXP_COMP);
  _VISCA_append_byte(&packet, VISCA_UP);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_exp_comp_down(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_EXP_COMP);
  _VISCA_append_byte(&packet, VISCA_DOWN);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_exp_comp_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_EXP_COMP);
  _VISCA_append_byte(&packet, VISCA_RESET);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);

  return VISCA_SUCCESS;
}


VISCA_API uint32_t
VISCA_set_exp_comp_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_EXP_COMP_VALUE);
  _VISCA_append_byte(&packet, (value & 0xF000) >> 12);
  _VISCA_append_byte(&packet, (value & 0x0F00) >>  8);
  _VISCA_append_byte(&packet, (value & 0x00F0) >>  4);
  _VISCA_append_byte(&packet, (value & 0x000F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_exp_comp_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_EXP_COMP_POWER);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_auto_exp_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t mode)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_AUTO_EXP);
  _VISCA_append_byte(&packet, mode);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_slow_shutter_auto(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_SLOW_SHUTTER);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_backlight_comp(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_BACKLIGHT_COMP);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_zero_lux_shot(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_ZERO_LUX);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_ir_led(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_IR_LED);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_wide_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t mode)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_WIDE_MODE);
  _VISCA_append_byte(&packet, mode);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_mirror(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_MIRROR);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_freeze(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FREEZE);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_picture_effect(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t mode)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_PICTURE_EFFECT);
  _VISCA_append_byte(&packet, mode);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_digital_effect(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t mode)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_DIGITAL_EFFECT);
  _VISCA_append_byte(&packet, mode);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_digital_effect_level(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t level)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_DIGITAL_EFFECT_LEVEL);
  _VISCA_append_byte(&packet, level);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_cam_stabilizer(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;
  
  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_CAM_STABILIZER);
  _VISCA_append_byte(&packet, power);
  
  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_memory_set(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t channel)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_MEMORY);
  _VISCA_append_byte(&packet, VISCA_MEMORY_SET);
  _VISCA_append_byte(&packet, channel);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_memory_recall(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t channel)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_MEMORY);
  _VISCA_append_byte(&packet, VISCA_MEMORY_RECALL);
  _VISCA_append_byte(&packet, channel);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_memory_reset(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t channel)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_MEMORY);
  _VISCA_append_byte(&packet, VISCA_MEMORY_RESET);
  _VISCA_append_byte(&packet, channel);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_display(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_DISPLAY);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_date_time(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_DATE_TIME_SET);
  _VISCA_append_byte(&packet, year/10);
  _VISCA_append_byte(&packet, year-10*(year/10));
  _VISCA_append_byte(&packet, month/10);
  _VISCA_append_byte(&packet, month-10*(month/10));
  _VISCA_append_byte(&packet, day/10);
  _VISCA_append_byte(&packet, day-10*(day/10));
  _VISCA_append_byte(&packet, hour/10);
  _VISCA_append_byte(&packet, hour-10*(hour/10));
  _VISCA_append_byte(&packet, minute/10);
  _VISCA_append_byte(&packet, minute-10*(minute/10));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_date_display(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_DATE_DISPLAY);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_time_display(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_TIME_DISPLAY);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_title_display(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_TITLE_DISPLAY);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_title_clear(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_TITLE_DISPLAY);
  _VISCA_append_byte(&packet, VISCA_TITLE_DISPLAY_CLEAR);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_title_params(VISCAInterface_t *iface, VISCACamera_t *camera, VISCATitleData_t *title)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_TITLE_SET);
  _VISCA_append_byte(&packet, VISCA_TITLE_SET_PARAMS);
  _VISCA_append_byte(&packet, title->vposition);
  _VISCA_append_byte(&packet, title->hposition);
  _VISCA_append_byte(&packet, title->color);
  _VISCA_append_byte(&packet, title->blink);
  _VISCA_append_byte(&packet, 0);
  _VISCA_append_byte(&packet, 0);
  _VISCA_append_byte(&packet, 0);
  _VISCA_append_byte(&packet, 0);
  _VISCA_append_byte(&packet, 0);
  _VISCA_append_byte(&packet, 0);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_title(VISCAInterface_t *iface, VISCACamera_t *camera, VISCATitleData_t *title)
{
  VISCAPacket_t packet;
  int i, err=0;
  
  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_TITLE_SET);
  _VISCA_append_byte(&packet, VISCA_TITLE_SET_PART1);

  for (i=0;i<10;i++)
    _VISCA_append_byte(&packet, title->title[i]);

  err+=_VISCA_send_packet_with_reply(iface, camera, &packet);

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_TITLE_SET);
  _VISCA_append_byte(&packet, VISCA_TITLE_SET_PART2);

  for (i=0;i<10;i++)
    _VISCA_append_byte(&packet, title->title[i+10]);

  err+=_VISCA_send_packet_with_reply(iface, camera, &packet);

  return err;
}


VISCA_API uint32_t
VISCA_set_spot_ae_on(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_SPOT_AE);
  _VISCA_append_byte(&packet, VISCA_SPOT_AE_ON);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_spot_ae_off(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_SPOT_AE);
  _VISCA_append_byte(&packet, VISCA_SPOT_AE_OFF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_spot_ae_position(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t x_position, uint8_t y_position)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_SPOT_AE_POSITION);
  _VISCA_append_byte(&packet, (x_position & 0xF0) >>  4);
  _VISCA_append_byte(&packet, (x_position & 0x0F));
  _VISCA_append_byte(&packet, (y_position & 0xF0) >>  4);
  _VISCA_append_byte(&packet, (y_position & 0x0F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


/***********************************/
/*       INQUIRY FUNCTIONS         */
/***********************************/

VISCA_API uint32_t
VISCA_get_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_POWER);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_dzoom(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_DZOOM);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else {
    *power=iface->ibuf[2];
    return VISCA_SUCCESS;
  }
}


VISCA_API uint32_t
VISCA_get_dzoom_limit(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_DZOOM_LIMIT);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else {
    *value=iface->ibuf[2];
    return VISCA_SUCCESS;
  }
}



VISCA_API uint32_t
VISCA_get_zoom_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_ZOOM_VALUE);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else {
    *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
      // qDebug() << Q_FUNC_INFO << iface->ibuf[2] << iface->ibuf[3] << iface->ibuf[4] << iface->ibuf[5] << (*value);
    return VISCA_SUCCESS;
  }

}


VISCA_API uint32_t
VISCA_get_focus_auto(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS_AUTO);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_focus_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS_VALUE);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
      // qDebug() << Q_FUNC_INFO << *value;
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_focus_auto_sense(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS_AUTO_SENSE );
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *mode=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_focus_near_limit(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FOCUS_NEAR_LIMIT);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_whitebal_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_WB);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *mode=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_rgain_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_RGAIN_VALUE);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_bgain_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_BGAIN_VALUE);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_auto_exp_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_AUTO_EXP);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *mode=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_slow_shutter_auto(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_SLOW_SHUTTER);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *mode=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_shutter_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_SHUTTER_VALUE);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      // qDebug()<<"bytes recv"<<iface->bytes;
      // for (int i = 0; i < iface->bytes; i++)
      //     qDebug()<<QString::number(iface->ibuf[i], 16).toUpper();
      *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_iris_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_IRIS_VALUE);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_gain_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_GAIN_VALUE);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_bright_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_BRIGHT_VALUE);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_exp_comp_power(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_EXP_COMP_POWER);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_exp_comp_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_EXP_COMP_VALUE);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_backlight_comp(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_BACKLIGHT_COMP);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_aperture_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_APERTURE_VALUE);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_zero_lux_shot(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_ZERO_LUX);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_ir_led(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_IR_LED);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_wide_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_WIDE_MODE);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *mode=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_mirror(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_MIRROR);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_freeze(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_FREEZE);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_picture_effect(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_PICTURE_EFFECT);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *mode=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_digital_effect(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *mode)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_DIGITAL_EFFECT);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *mode=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_digital_effect_level(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_DIGITAL_EFFECT_LEVEL);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *value=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_memory(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *channel)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_MEMORY);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *channel=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_display(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_DISPLAY);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_id(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *id)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_ID);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *id=(iface->ibuf[2]<<12)+(iface->ibuf[3]<<8)+(iface->ibuf[4]<<4)+iface->ibuf[5];
      return VISCA_SUCCESS;
    }
}
VISCA_API uint32_t
VISCA_set_irreceive_on(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_IRRECEIVE);
  _VISCA_append_byte(&packet, VISCA_IRRECEIVE_ON);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_irreceive_off(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_IRRECEIVE);
  _VISCA_append_byte(&packet, VISCA_IRRECEIVE_OFF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_irreceive_onoff(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_IRRECEIVE);
  _VISCA_append_byte(&packet, VISCA_IRRECEIVE_ONOFF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_pantilt_up(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t pan_speed, uint32_t tilt_speed)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE);
  _VISCA_append_byte(&packet, pan_speed);
  _VISCA_append_byte(&packet, tilt_speed);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_HORIZ_STOP);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_VERT_UP);
  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_pantilt_down(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t pan_speed, uint32_t tilt_speed)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE);
  _VISCA_append_byte(&packet, pan_speed);
  _VISCA_append_byte(&packet, tilt_speed);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_HORIZ_STOP);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_VERT_DOWN);
  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_pantilt_left(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t pan_speed, uint32_t tilt_speed)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE);
  _VISCA_append_byte(&packet, pan_speed);
  _VISCA_append_byte(&packet, tilt_speed);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_HORIZ_LEFT);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_VERT_STOP);
  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_pantilt_right(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t pan_speed, uint32_t tilt_speed)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE);
  _VISCA_append_byte(&packet, pan_speed);
  _VISCA_append_byte(&packet, tilt_speed);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_HORIZ_RIGHT);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_VERT_STOP);
  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_pantilt_upleft(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t pan_speed, uint32_t tilt_speed)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE);
  _VISCA_append_byte(&packet, pan_speed);
  _VISCA_append_byte(&packet, tilt_speed);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_HORIZ_LEFT);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_VERT_UP);
  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_pantilt_upright(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t pan_speed, uint32_t tilt_speed)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE);
  _VISCA_append_byte(&packet, pan_speed);
  _VISCA_append_byte(&packet, tilt_speed);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_HORIZ_RIGHT);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_VERT_UP);
  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_pantilt_downleft(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t pan_speed, uint32_t tilt_speed)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE);
  _VISCA_append_byte(&packet, pan_speed);
  _VISCA_append_byte(&packet, tilt_speed);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_HORIZ_LEFT);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_VERT_DOWN);
  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_pantilt_downright(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t pan_speed, uint32_t tilt_speed)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE);
  _VISCA_append_byte(&packet, pan_speed);
  _VISCA_append_byte(&packet, tilt_speed);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_HORIZ_RIGHT);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_VERT_DOWN);
  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_pantilt_stop(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t pan_speed, uint32_t tilt_speed)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE);
  _VISCA_append_byte(&packet, pan_speed);
  _VISCA_append_byte(&packet, tilt_speed);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_HORIZ_STOP);
  _VISCA_append_byte(&packet, VISCA_PT_DRIVE_VERT_STOP);
  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_pantilt_absolute_position(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t pan_speed, uint32_t tilt_speed, int pan_position, int tilt_position)
{
  VISCAPacket_t packet;

  uint32_t pan_pos=(uint32_t) pan_position;
  uint32_t tilt_pos=(uint32_t) tilt_position;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_ABSOLUTE_POSITION);
  _VISCA_append_byte(&packet, pan_speed);
  _VISCA_append_byte(&packet, tilt_speed);

  _VISCA_append_byte(&packet, (pan_pos & 0x0f000) >> 12);
  _VISCA_append_byte(&packet, (pan_pos & 0x00f00) >>  8);
  _VISCA_append_byte(&packet, (pan_pos & 0x000f0) >>  4);
  _VISCA_append_byte(&packet,  pan_pos & 0x0000f       );

  _VISCA_append_byte(&packet, (tilt_pos & 0xf000) >> 12);
  _VISCA_append_byte(&packet, (tilt_pos & 0x0f00) >> 8);
  _VISCA_append_byte(&packet, (tilt_pos & 0x00f0) >> 4);
  _VISCA_append_byte(&packet, tilt_pos & 0x000f);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_pantilt_relative_position(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t pan_speed, uint32_t tilt_speed, int pan_position, int tilt_position)
{
  VISCAPacket_t packet;

  uint32_t pan_pos=(uint32_t) pan_position;
  uint32_t tilt_pos=(uint32_t) tilt_position;
  

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_RELATIVE_POSITION);
  _VISCA_append_byte(&packet, pan_speed);
  _VISCA_append_byte(&packet, tilt_speed);

  _VISCA_append_byte(&packet, (pan_pos & 0x0f000) >> 12);
  _VISCA_append_byte(&packet, (pan_pos & 0x00f00) >>  8);
  _VISCA_append_byte(&packet, (pan_pos & 0x000f0) >>  4);
  _VISCA_append_byte(&packet,  pan_pos & 0x0000f       );

  _VISCA_append_byte(&packet, (tilt_pos & 0xf000) >> 12);
  _VISCA_append_byte(&packet, (tilt_pos & 0x0f00) >> 8);
  _VISCA_append_byte(&packet, (tilt_pos & 0x00f0) >> 4);
  _VISCA_append_byte(&packet, tilt_pos & 0x000f);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_pantilt_home(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_HOME);
  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_pantilt_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_RESET);
  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_pantilt_limit_upright(VISCAInterface_t *iface, VISCACamera_t *camera, int pan_position, int tilt_position)
{
  VISCAPacket_t packet;

  uint32_t pan_pos=(uint32_t) pan_position;
  uint32_t tilt_pos=(uint32_t) tilt_position;
  

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_LIMITSET);
  _VISCA_append_byte(&packet, VISCA_PT_LIMITSET_SET);
  _VISCA_append_byte(&packet, VISCA_PT_LIMITSET_SET_UR);
  _VISCA_append_byte(&packet, (pan_pos & 0xf000) >> 12);
  _VISCA_append_byte(&packet, (pan_pos & 0x0f00) >> 8);
  _VISCA_append_byte(&packet, (pan_pos & 0x00f0) >> 4);
  _VISCA_append_byte(&packet, pan_pos & 0x000f);

  _VISCA_append_byte(&packet, (tilt_pos & 0xf000) >> 12);
  _VISCA_append_byte(&packet, (tilt_pos & 0x0f00) >> 8);
  _VISCA_append_byte(&packet, (tilt_pos & 0x00f0) >> 4);
  _VISCA_append_byte(&packet, tilt_pos & 0x000f);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_pantilt_limit_downleft(VISCAInterface_t *iface, VISCACamera_t *camera, int pan_position, int tilt_position)
{
  VISCAPacket_t packet;

  uint32_t pan_pos=(uint32_t) pan_position;
  uint32_t tilt_pos=(uint32_t) tilt_position;
  

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_LIMITSET);
  _VISCA_append_byte(&packet, VISCA_PT_LIMITSET_SET);
  _VISCA_append_byte(&packet, VISCA_PT_LIMITSET_SET_DL);
  _VISCA_append_byte(&packet, (pan_pos & 0xf000) >> 12);
  _VISCA_append_byte(&packet, (pan_pos & 0x0f00) >> 8);
  _VISCA_append_byte(&packet, (pan_pos & 0x00f0) >> 4);
  _VISCA_append_byte(&packet, pan_pos & 0x000f);

  _VISCA_append_byte(&packet, (tilt_pos & 0xf000) >> 12);
  _VISCA_append_byte(&packet, (tilt_pos & 0x0f00) >> 8);
  _VISCA_append_byte(&packet, (tilt_pos & 0x00f0) >> 4);
  _VISCA_append_byte(&packet, tilt_pos & 0x000f);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_pantilt_limit_downleft_clear(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  uint32_t pan_pos=0x7fff;
  uint32_t tilt_pos=0x7fff;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_LIMITSET);
  _VISCA_append_byte(&packet, VISCA_PT_LIMITSET_CLEAR);
  _VISCA_append_byte(&packet, VISCA_PT_LIMITSET_SET_DL);
  _VISCA_append_byte(&packet, (pan_pos & 0xf000) >> 12);
  _VISCA_append_byte(&packet, (pan_pos & 0x0f00) >> 8);
  _VISCA_append_byte(&packet, (pan_pos & 0x00f0) >> 4);
  _VISCA_append_byte(&packet, pan_pos & 0x000f);

  _VISCA_append_byte(&packet, (tilt_pos & 0xf000) >> 12);
  _VISCA_append_byte(&packet, (tilt_pos & 0x0f00) >> 8);
  _VISCA_append_byte(&packet, (tilt_pos & 0x00f0) >> 4);
  _VISCA_append_byte(&packet, tilt_pos & 0x000f);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_pantilt_limit_upright_clear(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  uint32_t pan_pos=0x7fff;
  uint32_t tilt_pos=0x7fff;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_LIMITSET);
  _VISCA_append_byte(&packet, VISCA_PT_LIMITSET_CLEAR);
  _VISCA_append_byte(&packet, VISCA_PT_LIMITSET_SET_UR);
  _VISCA_append_byte(&packet, (pan_pos & 0xf000) >> 12);
  _VISCA_append_byte(&packet, (pan_pos & 0x0f00) >> 8);
  _VISCA_append_byte(&packet, (pan_pos & 0x00f0) >> 4);
  _VISCA_append_byte(&packet, pan_pos & 0x000f);

  _VISCA_append_byte(&packet, (tilt_pos & 0xf000) >> 12);
  _VISCA_append_byte(&packet, (tilt_pos & 0x0f00) >> 8);
  _VISCA_append_byte(&packet, (tilt_pos & 0x00f0) >> 4);
  _VISCA_append_byte(&packet, tilt_pos & 0x000f);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_datascreen_on(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_DATASCREEN);
  _VISCA_append_byte(&packet, VISCA_PT_DATASCREEN_ON);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_datascreen_off(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_DATASCREEN);
  _VISCA_append_byte(&packet, VISCA_PT_DATASCREEN_OFF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_datascreen_onoff(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_DATASCREEN);
  _VISCA_append_byte(&packet, VISCA_PT_DATASCREEN_ONOFF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_register(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t reg_num, uint8_t reg_val)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_REGISTER_VALUE);
  _VISCA_append_byte(&packet, reg_num);
  _VISCA_append_byte(&packet, (reg_val & 0xF0) >>  4);
  _VISCA_append_byte(&packet, (reg_val & 0x0F));
  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


/***********************************/
/*       INQUIRY FUNCTIONS         */
/***********************************/

VISCA_API uint32_t
VISCA_get_videosystem(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *system)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_VIDEOSYSTEM_INQ);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *system=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}

VISCA_API uint32_t
VISCA_get_pantilt_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *status)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_MODE_INQ);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *status = ((iface->ibuf[2] & 0xff) << 8) + (iface->ibuf[3] & 0xff);
      return VISCA_SUCCESS;
    }
}

VISCA_API uint32_t
VISCA_get_pantilt_maxspeed(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *max_pan_speed, uint8_t *max_tilt_speed)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_MAXSPEED_INQ);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *max_pan_speed = (iface->ibuf[2] & 0xff);
      *max_tilt_speed = (iface->ibuf[3] & 0xff);
      return VISCA_SUCCESS;
    }
}

VISCA_API uint32_t
VISCA_get_pantilt_position(VISCAInterface_t *iface, VISCACamera_t *camera, int16_t *pan_position, int16_t *tilt_position)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_POSITION_INQ);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *pan_position  = ((iface->ibuf[2] & 0xf) << 12) + ((iface->ibuf[3] & 0xf) << 8) + ((iface->ibuf[4] & 0xf) << 4) + (iface->ibuf[5] & 0xf);
      *tilt_position = ((iface->ibuf[6] & 0xf) << 12) + ((iface->ibuf[7] & 0xf) << 8) + ((iface->ibuf[8] & 0xf) << 4) + (iface->ibuf[9] & 0xf);

      return VISCA_SUCCESS;
    }
}

VISCA_API uint32_t
VISCA_get_datascreen(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *status)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_PT_DATASCREEN_INQ);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *status=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_register(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t reg_num, uint8_t* reg_val)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_REGISTER_VALUE);
  _VISCA_append_byte(&packet, reg_num);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *reg_val=(iface->ibuf[2]<<4)+iface->ibuf[3];
      return VISCA_SUCCESS;
    }
}


/********************************/
/* SPECIAL FUNCTIONS FOR D30/31 */
/********************************/

VISCA_API uint32_t
VISCA_set_wide_con_lens(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_WIDE_CON_LENS);
  _VISCA_append_byte(&packet, VISCA_WIDE_CON_LENS_SET);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_at_mode_onoff(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_AT_MODE);
  _VISCA_append_byte(&packet, VISCA_AT_ONOFF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_at_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_AT_MODE);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_at_ae_onoff(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_AT_AE);
  _VISCA_append_byte(&packet, VISCA_AT_ONOFF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_at_ae(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_AT_AE);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_at_autozoom_onoff(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_AT_AUTOZOOM);
  _VISCA_append_byte(&packet, VISCA_AT_ONOFF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_at_autozoom(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_AT_AUTOZOOM);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_atmd_framedisplay_onoff(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_ATMD_FRAMEDISPLAY);
  _VISCA_append_byte(&packet, VISCA_AT_ONOFF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_atmd_framedisplay(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_ATMD_FRAMEDISPLAY);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_at_frameoffset_onoff(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_AT_FRAMEOFFSET);
  _VISCA_append_byte(&packet, VISCA_AT_ONOFF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_at_frameoffset(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_AT_FRAMEOFFSET);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_atmd_startstop(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_ATMD_STARTSTOP);
  _VISCA_append_byte(&packet, VISCA_AT_ONOFF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_at_chase(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_AT_CHASE);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_at_chase_next(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_AT_CHASE);
  _VISCA_append_byte(&packet, VISCA_AT_CHASE_NEXT);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_md_mode_onoff(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_MODE);
  _VISCA_append_byte(&packet, VISCA_MD_ONOFF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_md_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_MODE);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_md_frame(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_FRAME);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_md_detect(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_DETECT);
  _VISCA_append_byte(&packet, VISCA_MD_ONOFF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_at_entry(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_AT_ENTRY);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_at_lostinfo(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_ATMD_LOSTINFO1);
  _VISCA_append_byte(&packet, VISCA_ATMD_LOSTINFO2);
  _VISCA_append_byte(&packet, VISCA_AT_LOSTINFO);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_md_lostinfo(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, VISCA_ATMD_LOSTINFO1);
  _VISCA_append_byte(&packet, VISCA_ATMD_LOSTINFO2);
  _VISCA_append_byte(&packet, VISCA_MD_LOSTINFO);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_md_adjust_ylevel(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST_YLEVEL);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_md_adjust_huelevel(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST_HUELEVEL);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_md_adjust_size(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST_SIZE);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_md_adjust_disptime(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST_DISPTIME);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_md_adjust_refmode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST_REFMODE);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_md_adjust_reftime(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST_REFTIME);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_md_measure_mode1_onoff(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_MEASURE_MODE_1);
  _VISCA_append_byte(&packet, VISCA_MD_ONOFF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_md_measure_mode1(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_MEASURE_MODE_1);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_md_measure_mode2_onoff(VISCAInterface_t *iface, VISCACamera_t *camera)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_MEASURE_MODE_2);
  _VISCA_append_byte(&packet, VISCA_MD_ONOFF);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_md_measure_mode2(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_MEASURE_MODE_2);
  _VISCA_append_byte(&packet, power);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_get_keylock(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_KEYLOCK);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_wide_con_lens(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_WIDE_CON_LENS);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_atmd_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_ATMD_MODE);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_at_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_AT_MODE_QUERY);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *value=((iface->ibuf[2] & 0xff) << 8) + (iface->ibuf[3] & 0xff);
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_at_entry(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_AT_ENTRY);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_md_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_MODE_QUERY);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *value=((iface->ibuf[2] & 0xff) << 8) + (iface->ibuf[3] & 0xff);
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_md_ylevel(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST_YLEVEL);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=(iface->ibuf[3] & 0x0f);
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_md_huelevel(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST_HUELEVEL);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=(iface->ibuf[3] & 0x0f);
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_md_size(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST_SIZE);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=(iface->ibuf[3] & 0x0f);
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_md_disptime(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST_DISPTIME);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=(iface->ibuf[3] & 0x0f);
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_md_refmode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_ADJUST_REFMODE);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=iface->ibuf[2];
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_md_reftime(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_REFTIME_QUERY);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *power=(iface->ibuf[3] & 0x0f);
      return VISCA_SUCCESS;
    }
}



VISCA_API uint32_t
VISCA_get_at_obj_pos(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *xpos, uint8_t *ypos, uint8_t *status)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_AT_POSITION);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *xpos=iface->ibuf[2];
      *ypos=iface->ibuf[3];
      *status=(iface->ibuf[4] & 0x0f);
      return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_get_md_obj_pos(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *xpos, uint8_t *ypos, uint8_t *status)
{
  VISCAPacket_t packet;
  uint32_t err;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_INQUIRY);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA2);
  _VISCA_append_byte(&packet, VISCA_MD_POSITION);
  err=_VISCA_send_packet_with_reply(iface, camera, &packet);
  if (err!=VISCA_SUCCESS)
    return err;
  else
    {
      *xpos=iface->ibuf[2];
      *ypos=iface->ibuf[3];
      *status=(iface->ibuf[4] & 0x0f);
      return VISCA_SUCCESS;
    }
}

VISCA_API uint32_t
VISCA_get_temperature(VISCAInterface_t *iface, VISCACamera_t *camera, int8_t *temp)
{
    VISCAPacket_t packet;
    uint32_t err;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_TEMPERATURE_INQ);
    err=_VISCA_send_packet_with_reply(iface, camera, &packet);
    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        // qDebug()<<"bytes recv"<<iface->bytes;
        // for (int i = 0; i < iface->bytes; i++)
        //     qDebug()<<QString::number(iface->ibuf[i], 16).toUpper();
        *temp = (iface->ibuf[4] << 4) + (iface->ibuf[5] & 0x0F);
        return VISCA_SUCCESS;
    }
}

VISCA_API uint32_t
VISCA_set_contrast_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_CONTRAST);
    _VISCA_append_byte(&packet, 0x01);
    _VISCA_append_byte(&packet, value>>4);
    _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_contrast(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_CONTRAST);
    err=_VISCA_send_packet_with_reply(iface, camera, &packet);
    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *value = (iface->ibuf[2] << 4) + (iface->ibuf[3] & 0x0F);
        return VISCA_SUCCESS;
    }
}

VISCA_API uint32_t
VISCA_set_on_defog_level(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value)
{
    VISCAPacket_t packet;

    if((value<1) && (value>3))
        return -1;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_DEFOG);
    _VISCA_append_byte(&packet, VISCA_DEFOG_ON);
    _VISCA_append_byte(&packet, value);
  //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_off_defog(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_DEFOG);
    _VISCA_append_byte(&packet, VISCA_DEFOG_OFF);
    _VISCA_append_byte(&packet, 0x00);
    //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}



VISCA_API uint32_t
VISCA_set_nr(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_NR);
    _VISCA_append_byte(&packet, value);
    //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_23dnr(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value)
{
    VISCAPacket_t packet;


    uint8_t ui2dnr = value&0x0F;
    uint8_t ui3dnr = value&0xF0>>4;
    if((ui2dnr>5)||(ui3dnr>5))
        return -1;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_NR);
    _VISCA_append_byte(&packet, ui2dnr);
    _VISCA_append_byte(&packet, ui3dnr);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}
}
VISCA_API uint32_t
VISCA_set_on_highsens(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_HIGHSENS);
    _VISCA_append_byte(&packet, VISCA_HIGHSENS_ON);
    _VISCA_append_byte(&packet, 0x00);
    //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_off_highsens(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_HIGHSENS);
    _VISCA_append_byte(&packet, VISCA_HIGHSENS_OFF);
    _VISCA_append_byte(&packet, 0x00);
    //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_on_min_shutter(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_MIN_SHUTTER);
    _VISCA_append_byte(&packet, VISCA_MIN_SHUTTER_ON);
    _VISCA_append_byte(&packet, 0x00);
    //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_off_min_shutter(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_MIN_SHUTTER);
    _VISCA_append_byte(&packet, VISCA_MIN_SHUTTER_OFF);
    _VISCA_append_byte(&packet, 0x00);
    //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_shutter_limit(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t value)
{
    VISCAPacket_t packet;

    if((value<1) && (value>3))
        return -1;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_MIN_SHUTTER_LIMIT);
    _VISCA_append_byte(&packet, value&0xF0>>4);
    _VISCA_append_byte(&packet, value&0x0F);
    //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_shutter_limit(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_MIN_SHUTTER_LIMIT);
    err=_VISCA_send_packet_with_reply(iface, camera, &packet);
    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *value = (iface->ibuf[2] << 4) + (iface->ibuf[3] & 0x0F);
        return VISCA_SUCCESS;
    }
}



VISCA_API uint32_t
VISCA_set_on_auto_zero_lux(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_AUTO_ZERO_LUX);
    _VISCA_append_byte(&packet, VISCA_AUTO_ZERO_LUX_ON);
    //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}
VISCA_API uint32_t
VISCA_set_on_color_auto_zero_lux(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_AUTO_ZERO_LUX);
    _VISCA_append_byte(&packet, VISCA_AUTO_ZERO_LUX_ON_COLOR);
    //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_off_auto_zero_lux(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_AUTO_ZERO_LUX);
    _VISCA_append_byte(&packet, VISCA_AUTO_ZERO_LUX_OFF);
    //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_auto_zero_lux_treshold(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t treshold)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_AUTO_ZERO_LUX_TH);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, (treshold & 0x00f0) >> 4);
    _VISCA_append_byte(&packet, treshold & 0x000f);

    //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_on_auto_zero_lux_alarm_reply(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_AUTO_ZERO_LUX_ALARM);
    _VISCA_append_byte(&packet, VISCA_AUTO_ZERO_LUX_ALARM_ON);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}
VISCA_API uint32_t
VISCA_set_off_auto_zero_lux_alarm_reply(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_AUTO_ZERO_LUX_ALARM);
    _VISCA_append_byte(&packet, VISCA_AUTO_ZERO_LUX_ALARM_OFF);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_normal_af_mode(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_AF_MODE);
    _VISCA_append_byte(&packet, VISCA_AF_MODE_NORMAL_AF);
    _VISCA_append_byte(&packet, 0x00);
    //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_interval_af_mode(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_AF_MODE);
    _VISCA_append_byte(&packet, VISCA_AF_MODE_INTERVAL_AF);
    //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_zoom_trig_af_mode(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_AF_MODE);
    _VISCA_append_byte(&packet, VISCA_AF_MODE_ZOOM_TRIG_AF);
    //  _VISCA_append_byte(&packet, value&0x0F);


    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_active_interval_af_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t move_time, uint16_t interval)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_AF_MODE_ACTIVE_INTERV_TIME);

    _VISCA_append_byte(&packet, (move_time & 0xf000) >> 12);
    _VISCA_append_byte(&packet, (move_time & 0x0f00) >> 8);
    _VISCA_append_byte(&packet, (move_time & 0x00f0) >> 4);
    _VISCA_append_byte(&packet, move_time & 0x000f);

    _VISCA_append_byte(&packet, (interval & 0xf000) >> 12);
    _VISCA_append_byte(&packet, (interval & 0x0f00) >> 8);
    _VISCA_append_byte(&packet, (interval & 0x00f0) >> 4);
    _VISCA_append_byte(&packet, interval & 0x000f);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_on_spot_focus(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_FOCUS);
    _VISCA_append_byte(&packet, VISCA_SPOT_FOCUS_ON);
    //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_spot_focus(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_FOCUS);

     err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *value = (iface->ibuf[2] & 0x0F);
        return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_set_off_spot_focus(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_FOCUS);
    _VISCA_append_byte(&packet, VISCA_SPOT_FOCUS_OFF);
    //  _VISCA_append_byte(&packet, value&0x0F);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_active_interval_af_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_FOCUS_SET_PARAM);
    _VISCA_append_byte(&packet, 0x00);

    _VISCA_append_byte(&packet, x1 & 0x000f);
    _VISCA_append_byte(&packet, y1 & 0x000f);
    _VISCA_append_byte(&packet, x2 & 0x000f);
    _VISCA_append_byte(&packet, y2 & 0x000f);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_active_interval_af_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *x1, uint8_t *y1, uint8_t *x2, uint8_t *y2)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_FOCUS_SET_PARAM);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *x1 = (iface->ibuf[2] & 0x0F);
        *y1 = (iface->ibuf[3] & 0x0F);
        *x2 = (iface->ibuf[4] & 0x0F);
        *y2 = (iface->ibuf[5] & 0x0F);

        return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_set_on_spot_focus_display(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_FOCUS_DISPLAY);
    _VISCA_append_byte(&packet, VISCA_SPOT_FOCUS_DISPLAY_ON);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_off_spot_focus_display(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_FOCUS_DISPLAY);
    _VISCA_append_byte(&packet, VISCA_SPOT_FOCUS_DISPLAY_OFF);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_spot_focus_display(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_FOCUS_DISPLAY);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *value = (iface->ibuf[2] & 0x0F);
        return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_set_standard_ir_correction(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_IR_CORR);
    _VISCA_append_byte(&packet, VISCA_IR_CORR_STANDARD);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_light_ir_correction(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_IR_CORR);
    _VISCA_append_byte(&packet, VISCA_IR_CORR_IR_LIGHT);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_ir_correction(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_IR_CORR);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *value = (iface->ibuf[2] & 0x0F);
        return VISCA_SUCCESS;
    }
}

VISCA_API uint32_t
VISCA_set_spot_awb_on(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_AWB);
    _VISCA_append_byte(&packet, VISCA_SPOT_AWB_ON);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_spot_awb_off(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_AWB);
    _VISCA_append_byte(&packet, VISCA_SPOT_AWB_OFF);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_spot_awb_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_AWB);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *value = (iface->ibuf[2] & 0x0F);
        return VISCA_SUCCESS;
    }
}



VISCA_API uint32_t
VISCA_set_spot_awb_position(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t x1_position, uint8_t y1_position, uint8_t x2_position, uint8_t y2_position)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_AWB_SET_PARAM);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, (x1_position & 0x0F));
    _VISCA_append_byte(&packet, (y1_position & 0x0F));
    _VISCA_append_byte(&packet, (x2_position & 0x0F));
    _VISCA_append_byte(&packet, (y2_position & 0x0F));
    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_spot_awb_parameter(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *x1_position, uint8_t *y1_position, uint8_t *x2_position, uint8_t *y2_position)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_AWB_SET_PARAM);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *x1_position = (iface->ibuf[2] & 0x0F);
        *y1_position = (iface->ibuf[3] & 0x0F);
        *x2_position = (iface->ibuf[4] & 0x0F);
        *y2_position = (iface->ibuf[5] & 0x0F);

        return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_set_spot_awb_display_on(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_AWB_DISPLAY);
    _VISCA_append_byte(&packet, VISCA_SPOT_AWB_DISPLAY_ON);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_get_spot_awb_display_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_AWB_DISPLAY);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *value = (iface->ibuf[2] & 0x0F);
        return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_set_wd_on(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_WD);
    _VISCA_append_byte(&packet, VISCA_WD_ON);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_wd_off(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_WD);
    _VISCA_append_byte(&packet, VISCA_WD_OFF);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_wd_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_WD);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *value = (iface->ibuf[2] & 0x0F);
        return VISCA_SUCCESS;
    }
}

VISCA_API uint32_t
VISCA_set_wd_parameters(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t brightness, uint8_t br_compensation_sel,uint8_t compensation_lvl  )
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_WD_SET_PARAM);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, (brightness & 0x0F));
    _VISCA_append_byte(&packet, (br_compensation_sel & 0x0F));
    _VISCA_append_byte(&packet, (compensation_lvl & 0x0F));
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);


    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_get_wd_parameters(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *brightness, uint8_t *br_compensation_sel,uint8_t *compensation_lvl  )
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x05);
    _VISCA_append_byte(&packet, VISCA_SPOT_AWB_SET_PARAM);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *brightness = (iface->ibuf[3] & 0x0F);
        *br_compensation_sel = (iface->ibuf[4] & 0x0F);
        *compensation_lvl = (iface->ibuf[5] & 0x0F);

        return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_set_freeze_on(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_FREEZE);
    _VISCA_append_byte(&packet, VISCA_FREEZE_ON);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_freeze_off(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_FREEZE);
    _VISCA_append_byte(&packet, VISCA_FREEZE_OFF);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_freeze_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_FREEZE);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *value = (iface->ibuf[2] & 0x0F);
        return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_set_flip_on(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_FLIP);
    _VISCA_append_byte(&packet, VISCA_FLIP_OFF);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_flip_off(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_FLIP);
    _VISCA_append_byte(&packet, VISCA_FLIP_OFF);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_flip_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_FLIP);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *value = (iface->ibuf[2] & 0x0F);
        return VISCA_SUCCESS;
    }
}


VISCA_API uint32_t
VISCA_set_mute_on(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_MUTE);
    _VISCA_append_byte(&packet, VISCA_MUTE_ON);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_mute_off(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_MUTE);
    _VISCA_append_byte(&packet, VISCA_MUTE_OFF);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_mute_onoff(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_MUTE);
    _VISCA_append_byte(&packet, VISCA_MUTE_ONOFF);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_mute_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_MUTE);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *value = (iface->ibuf[2] & 0x0F);
        return VISCA_SUCCESS;
    }
}



VISCA_API uint32_t
VISCA_set_continous_zoompos_reply_on(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_CONT_ZOOM_POS_REPLY);
    _VISCA_append_byte(&packet, VISCA_CONT_ZOOM_POS_REPLY_ON);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_continous_zoompos_reply_off(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_CONT_ZOOM_POS_REPLY);
    _VISCA_append_byte(&packet, VISCA_CONT_ZOOM_POS_REPLY_OFF);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_continous_zoompos_reply_mode(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_CONT_ZOOM_POS_REPLY);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *value = (iface->ibuf[2] & 0x0F);
        return VISCA_SUCCESS;
    }

}


VISCA_API uint32_t
VISCA_set_continous_zoompos_reply_interval(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t v_interval)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_CONT_ZOOM_POS_INTERVAL);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, (v_interval & 0x00f0) >> 4);
    _VISCA_append_byte(&packet, v_interval & 0x000f);


    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_continous_zoompos_reply_interval(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *v_interval)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_CONT_ZOOM_POS_INTERVAL);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *v_interval  = ((iface->ibuf[4] & 0xf) << 4) + (iface->ibuf[5] & 0xf);
        return VISCA_SUCCESS;
    }

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}



VISCA_API uint32_t
VISCA_set_chroma_suppress(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t chroma)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_CHROMA_SUPRESS);
    _VISCA_append_byte(&packet, chroma & 0x000f);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_chroma_suppress(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_CHROMA_SUPRESS);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *value = (iface->ibuf[2] & 0xFF);
        return VISCA_SUCCESS;
    }

}


VISCA_API uint32_t
VISCA_set_color_gain_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t color_gain)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_COLOR_GAIN_VALUE);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, color_gain & 0x000f);
    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_color_gain_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_COLOR_GAIN_VALUE);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *value = (iface->ibuf[5] & 0xFF);
        return VISCA_SUCCESS;
    }

}
VISCA_API uint32_t
VISCA_set_color_hue_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t color_gain)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_COLOR_HUE_VALUE);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, color_gain & 0x000f);
    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_color_hue_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_COLOR_HUE_VALUE);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        *value = (iface->ibuf[5] & 0xFF);
        return VISCA_SUCCESS;
    }

}

VISCA_API uint32_t
VISCA_set_gamma_offset_value(VISCAInterface_t *iface, VISCACamera_t *camera, int8_t gamma_offset)
{
    uint8_t gamma_offset_sign = (gamma_offset < 0)? 1:0;

    uint16_t gamma_offset_pos = abs(gamma_offset);
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_GAMMA_OFFSET_VALUE);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, gamma_offset_sign);
    _VISCA_append_byte(&packet, (gamma_offset_pos & 0x00f0) >> 4);
    _VISCA_append_byte(&packet, gamma_offset_pos & 0x000f);
    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_get_gamma_offset_value(VISCAInterface_t *iface, VISCACamera_t *camera, int16_t *value)
{
    VISCAPacket_t packet;
    uint32_t err;
    int16_t gamma_offset=0;
    int16_t gamma_sign=0;


    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_INQUIRY);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_GAMMA_OFFSET_VALUE);

    err=_VISCA_send_packet_with_reply(iface, camera, &packet);

    if (err!=VISCA_SUCCESS)
        return err;
    else
    {
        gamma_offset  = ((iface->ibuf[6] & 0xf) << 4) + (iface->ibuf[7] & 0xf);
        gamma_sign= (iface->ibuf[5] & 0x1) * -1;

        *value = gamma_sign * gamma_offset;
        return VISCA_SUCCESS;
    }

}


VISCA_API uint32_t
VISCA_set_contr_adj_value(VISCAInterface_t *iface, VISCACamera_t *camera, int8_t gamma_offset)
{
    uint8_t gamma_offset_sign = (gamma_offset < 0)? 1:0;

    uint16_t gamma_offset_pos = abs(gamma_offset);
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, 0x04);
    _VISCA_append_byte(&packet, VISCA_GAMMA_OFFSET_VALUE);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, gamma_offset_sign);
    _VISCA_append_byte(&packet, (gamma_offset_pos & 0x00f0) >> 4);
    _VISCA_append_byte(&packet, gamma_offset_pos & 0x000f);
    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}



VISCA_API uint32_t
VISCA_set_ex_exp_comp_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_EX_EXP_COMP);
    _VISCA_append_byte(&packet, VISCA_EX_EXP_COMP_1B);
    _VISCA_append_byte(&packet, VISCA_EX_EXP_COMP_RESET);
    _VISCA_append_byte(&packet, 0x00);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);

    return VISCA_SUCCESS;
}


VISCA_API uint32_t
VISCA_set_ex_exp_comp_up(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t step)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_EX_EXP_COMP);
    _VISCA_append_byte(&packet, VISCA_EX_EXP_COMP_1B);
    _VISCA_append_byte(&packet, VISCA_EX_EXP_COMP_UP);
    _VISCA_append_byte(&packet, step & 0x000f);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_exp_comp_down(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t step)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_EX_EXP_COMP);
    _VISCA_append_byte(&packet, VISCA_EX_EXP_COMP_1B);
    _VISCA_append_byte(&packet, VISCA_EX_EXP_COMP_DOWN);
    _VISCA_append_byte(&packet, step & 0x000f);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}




VISCA_API uint32_t
VISCA_set_ex_exp_comp_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_EX_EXP_COMP);
    _VISCA_append_byte(&packet, VISCA_EX_EXP_COMP_DIRECT);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, (value & 0x00F0) >>  4);
    _VISCA_append_byte(&packet, (value & 0x000F));

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_ex_aperture_reset(VISCAInterface_t *iface, VISCACamera_t *camera)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_EX_APERTURE);
    _VISCA_append_byte(&packet, VISCA_EX_APERTURE_1B);
    _VISCA_append_byte(&packet, VISCA_EX_APERTURE_RESET);
    _VISCA_append_byte(&packet, 0x00);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);

    return VISCA_SUCCESS;
}


VISCA_API uint32_t
VISCA_set_ex_aperture_up(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t step)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_EX_APERTURE);
    _VISCA_append_byte(&packet, VISCA_EX_APERTURE_1B);
    _VISCA_append_byte(&packet, VISCA_EX_APERTURE_UP);
    _VISCA_append_byte(&packet, step & 0x000f);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_ex_aperture_down(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t step)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_EX_APERTURE);
    _VISCA_append_byte(&packet, VISCA_EX_APERTURE_1B);
    _VISCA_append_byte(&packet, VISCA_EX_APERTURE_DOWN);
    _VISCA_append_byte(&packet, step & 0x000f);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}




VISCA_API uint32_t
VISCA_set_ex_aperture_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_EX_EXP_COMP);
    _VISCA_append_byte(&packet, VISCA_EX_EXP_COMP_DIRECT);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, (value & 0x00F0) >>  4);
    _VISCA_append_byte(&packet, (value & 0x000F));

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_ex_auto_zero_lux_threshold_off_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t th)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_EX_AUTO_ZERO_LUX);
    _VISCA_append_byte(&packet, VISCA_EX_AUTO_ZERO_LUX_1B);
    _VISCA_append_byte(&packet, VISCA_EX_AUTO_ZERO_TH_OFF_LEVEL);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, (th & 0x00F0) >>  4);
    _VISCA_append_byte(&packet, th & 0x000f);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}



VISCA_API uint32_t
VISCA_set_ex_auto_zero_lux_threshold_on_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t th)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_EX_AUTO_ZERO_LUX);
    _VISCA_append_byte(&packet, VISCA_EX_AUTO_ZERO_LUX_1B);
    _VISCA_append_byte(&packet, VISCA_EX_AUTO_ZERO_TH_ON_LEVEL);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, (th & 0x00F0) >>  4);
    _VISCA_append_byte(&packet, th & 0x000f);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_ex_color_gain_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t gain)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_EX_COLOR_GAIN_VALUE);
    _VISCA_append_byte(&packet, VISCA_EX_COLOR_GAIN_VALUE_1B);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, (gain & 0x00F0) >>  4);
    _VISCA_append_byte(&packet, gain & 0x000f);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_ex_color_hue_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t hue)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_EX_COLOR_HUE_VALUE);
    _VISCA_append_byte(&packet, VISCA_EX_COLOR_HUE_VALUE_1B);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, 0x00);
    _VISCA_append_byte(&packet, (hue & 0x00F0) >>  4);
    _VISCA_append_byte(&packet, hue & 0x000f);

    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_hlc_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t hlc_level, uint8_t mask_level)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_HLC_MASK_LEVEL);
    _VISCA_append_byte(&packet, hlc_level & 0x000f);
    _VISCA_append_byte(&packet, mask_level & 0x000f);
    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_flicker_reduction_on(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t hlc_level, uint8_t mask_level)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_FLICKER_REDUCTION);
    _VISCA_append_byte(&packet, VISCA_FLICKER_REDUCTION_ON);

    _VISCA_append_byte(&packet, hlc_level & 0x000f);
    _VISCA_append_byte(&packet, mask_level & 0x000f);
    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_set_flicker_reduction_off(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t hlc_level, uint8_t mask_level)
{
    VISCAPacket_t packet;

    _VISCA_init_packet(&packet);
    _VISCA_append_byte(&packet, VISCA_COMMAND);
    _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
    _VISCA_append_byte(&packet, VISCA_FLICKER_REDUCTION);
    _VISCA_append_byte(&packet, VISCA_FLICKER_REDUCTION_OFF);

    _VISCA_append_byte(&packet, hlc_level & 0x000f);
    _VISCA_append_byte(&packet, mask_level & 0x000f);
    return _VISCA_send_packet_with_reply(iface, camera, &packet);
}
