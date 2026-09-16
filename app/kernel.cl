
// __kernel void vector_add(__global const float* A, __global const float* B, __global float* C)
// {
//     int i = get_global_id(0);
//     C[i] = A[i] + B[i];

// }

__kernel void yuyv_to_rgb888(__global const uchar* yuyv, __global uchar* rgb, int width, int height)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    // Индекс текущего пикселя
    int index = (y * width + x) * 2; // Каждый YUYV пиксель занимает 2 байта

    // Проверка границ
    if (x < width && y < height) {
        // Извлечение Y, U и V значений
        /*uchar Y1 = yuyv[index];
        uchar U = yuyv[index + 1];
        uchar Y2 = yuyv[index + 2];
        uchar V = yuyv[index + 3];*/
        uchar Y1 = yuyv[index+3];
        uchar U = yuyv[index + 2];
        uchar Y2 = yuyv[index + 1];
        uchar V = yuyv[index + 0];


        // Преобразование YUV в RGB для первого пикселя
        int C1 = Y1 - 16;
        int D1 = U - 128;
        int E1 = V - 128;

        uchar R1 = clamp((298 * C1 + 409 * E1 + 128) >> 8, 0, 255);
        uchar G1 = clamp((298 * C1 - 100 * D1 - 208 * E1 + 128) >> 8, 0, 255);
        uchar B1 = clamp((298 * C1 + 516 * D1 + 128) >> 8, 0, 255);

        // Преобразование YUV в RGB для второго пикселя
        int C2 = Y2 - 16;
        int D2 = U - 128;
        int E2 = V - 128;

        uchar R2 = clamp((298 * C2 + 409 * E2 + 128) >> 8, 0, 255);
        uchar G2 = clamp((298 * C2 - 100 * D2 - 208 * E2 + 128) >> 8, 0, 255);
        uchar B2 = clamp((298 * C2 + 516 * D2 + 128) >> 8, 0, 255);

        // Запись результатов в RGB массив
        int rgbIndex1 = (y * width + x) * 3; // RGB занимает по одному байту на каждый цвет
        rgb[rgbIndex1]     = R1;   // Red
        rgb[rgbIndex1 + 1] = G1;   // Green
        rgb[rgbIndex1 + 2] = B1;   // Blue

        int rgbIndex2 = (y * width + x + 1) * 3;
        rgb[rgbIndex2]     = R2;
        rgb[rgbIndex2 + 1] = G2;
        rgb[rgbIndex2 + 2] = B2;
    }

}
