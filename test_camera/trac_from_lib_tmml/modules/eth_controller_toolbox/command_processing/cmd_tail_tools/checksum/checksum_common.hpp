#ifndef CHECKSUM_COMMON_HPP
#define CHECKSUM_COMMON_HPP

enum ChecksumCodecTypes
{
    CHECKSUM_CODEC_PASS,      // формально имитирует работу добавления и проверки контрольной суммы, но в действительности НЕ пишет и НЕ проверят
    CHECKSUM_CODEC_XOR        // исключающее "или"
};

#endif // CHECKSUM_COMMON_HPP