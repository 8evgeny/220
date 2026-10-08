#ifndef COLORS_H
#define COLORS_H

#include <opencv2/core/core.hpp>
#include <vector>

namespace color {
const cv::Scalar red          =  cv::Scalar(0, 0, 255);
const cv::Scalar blue         =  cv::Scalar(255, 0, 0);
const cv::Scalar green        =  cv::Scalar(0, 128, 0);
const cv::Scalar white        =  cv::Scalar(255, 255, 255);
const cv::Scalar gray        =  cv::Scalar(128, 128, 128);
const cv::Scalar black        =  cv::Scalar(0, 0, 0);
const cv::Scalar viol         =  cv::Scalar(255, 0, 255);
const cv::Scalar cyan         =  cv::Scalar(255, 255, 0);
const cv::Scalar yell         =  cv::Scalar(0, 255, 255);
const cv::Scalar orange       =  cv::Scalar(0,165,255);
const cv::Scalar darkorange   =  cv::Scalar(0,140,255);
const cv::Scalar brown        =  cv::Scalar(42,42,165);

namespace Green
{
const cv::Scalar chartreuse        =  cv::Scalar(0, 255, 127);
const cv::Scalar lime              =  cv::Scalar(0, 255, 0);
const cv::Scalar greenyellow       =  cv::Scalar(47, 173, 47);
const cv::Scalar mediumspringgreen =  cv::Scalar(154, 250, 0);
const cv::Scalar palegreen         =  cv::Scalar(152, 251, 152);
const cv::Scalar mediumseagreen    =  cv::Scalar(113, 179, 60);
const cv::Scalar seagreen          =  cv::Scalar(87, 139, 46);
const cv::Scalar darkolivegreen    =  cv::Scalar(47, 107, 85);
}

namespace Blue
{
const cv::Scalar powderblue        =  cv::Scalar(230, 224, 176);
const cv::Scalar lightskyblue      =  cv::Scalar(250, 206, 135);
const cv::Scalar deepskyblue       =  cv::Scalar(255, 191, 0);
const cv::Scalar dodgerblue        =  cv::Scalar(255, 144,30);
const cv::Scalar steelblue         =  cv::Scalar(180, 130, 70 );
const cv::Scalar slateblue         =  cv::Scalar(205, 90 , 106);
const cv::Scalar royalblue         =  cv::Scalar(225, 105, 65 );
const cv::Scalar mediumblue        =  cv::Scalar(205, 0  , 0  );
}

namespace Orange
{
const cv::Scalar coral       =  cv::Scalar(80, 127, 255);
const cv::Scalar tomato      =  cv::Scalar(71, 99 , 255);
const cv::Scalar orangered   =  cv::Scalar(0 , 69 , 255);
const cv::Scalar gold        =  cv::Scalar(0 , 215, 255);
const cv::Scalar orange      =  cv::Scalar(0 , 165, 255);
const cv::Scalar darkorange  =  cv::Scalar(0 , 140, 255);
}

namespace Purple
{
const cv::Scalar indigo        =  cv::Scalar(130, 0  , 75 );
const cv::Scalar darkmagenta   =  cv::Scalar(139, 0  , 139);
const cv::Scalar darkviolet    =  cv::Scalar(211, 0  , 148);
const cv::Scalar mediumpurple  =  cv::Scalar(219, 112, 147);
const cv::Scalar magenta       =  cv::Scalar(255, 0  , 255);
const cv::Scalar orchid        =  cv::Scalar(214, 112, 218);
const cv::Scalar plum          =  cv::Scalar(221, 160, 221);
}

const std::vector<cv::Scalar> greenColors = {color::Green::chartreuse       ,
                                             color::Green::lime             ,
                                             color::Green::greenyellow      ,
                                             color::Green::mediumspringgreen,
                                             color::Green::palegreen        ,
                                             color::Green::mediumseagreen   ,
                                             color::Green::seagreen         ,
                                             color::Green::darkolivegreen    };

const std::vector<cv::Scalar> blueColors = {color::Blue::powderblue  ,
                                            color::Blue::lightskyblue,
                                            color::Blue::deepskyblue ,
                                            color::Blue::dodgerblue  ,
                                            color::Blue::steelblue   ,
                                            color::Blue::slateblue   ,
                                            color::Blue::royalblue   ,
                                            color::Blue::mediumblue   };

const std::vector<cv::Scalar> orangeColors = {color::Orange::coral     ,
                                              color::Orange::tomato    ,
                                              color::Orange::orangered ,
                                              color::Orange::gold      ,
                                              color::Orange::orange    ,
                                              color::Orange::darkorange };

const std::vector<cv::Scalar> purpleColors = {color::Purple::indigo      ,
                                              color::Purple::darkmagenta ,
                                              color::Purple::darkviolet  ,
                                              color::Purple::mediumpurple,
                                              color::Purple::magenta     ,
                                              color::Purple::orchid      ,
                                              color::Purple::plum         };
}

#endif // COLORS_H
