#ifndef SIMPLE_EXPONENTIAL_FILTER_H
#define SIMPLE_EXPONENTIAL_FILTER_H

/**
 * Простой экспоненциальный фильтр для сглаживания значений
 */
class SimpleExponentialFilter {
private:
    float alpha_;           // Коэффициент сглаживания (0 < alpha <= 1)
    float filtered_value_;  // Текущее отфильтрованное значение
    bool initialized_;       // Флаг инициализации
    
public:
    /**
     * Конструктор
     * @param alpha Коэффициент сглаживания (0 < alpha <= 1)
     *              alpha близко к 1 - быстрая реакция, слабое сглаживание
     *              alpha близко к 0 - медленная реакция, сильное сглаживание
     */
    SimpleExponentialFilter(float alpha = 0.1)
        : alpha_(alpha), filtered_value_(0.0), initialized_(false) {
        if (alpha <= 0.0 || alpha > 1.0) {
            alpha_ = 0.1; // Значение по умолчанию
        }
    }
    
    /**
     * Обновление фильтра новым измерением
     * @param measurement Новое измерение
     * @return Отфильтрованное значение
     */
    float update(float measurement) {
        if (!initialized_) {
            filtered_value_ = measurement;
            initialized_ = true;
        } else {
            filtered_value_ = alpha_ * measurement + (1.0 - alpha_) * filtered_value_;
        }
        return filtered_value_;
    }
    
    /**
     * Получить текущее отфильтрованное значение
     */
    double getValue() const {
        return filtered_value_;
    }
    
    /**
     * Сбросить фильтр
     */
    void reset() {
        initialized_ = false;
        filtered_value_ = 0.0;
    }
    
    /**
     * Установить новый коэффициент сглаживания
     */
    void setAlpha(float alpha) {
        if (alpha > 0.0 && alpha <= 1.0) {
            alpha_ = alpha;
        }
    }
    
    /**
     * Получить коэффициент сглаживания
     */
    double getAlpha() const {
        return alpha_;
    }
    
    /**
     * Проверить, инициализирован ли фильтр
     */
    bool isInitialized() const {
        return initialized_;
    }
};

#endif // SIMPLE_EXPONENTIAL_FILTER_H
