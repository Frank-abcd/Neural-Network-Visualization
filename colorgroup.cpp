#include "colorgroup.h"

ColorGroup::ColorGroup() {
    // 初始化所有颜色主题
    m_themes = {
        //神经元 神经元边框 高权重 低权重 文本
        {"Classic",
         {QColor("#AEDCF0"), Qt::black, Qt::gray, Qt::gray,Qt::black}},
        {"Dark",
         {QColor(45, 45, 45), QColor(100, 100, 100), QColor(150, 150, 150), Qt::gray,Qt::white}},
        {"Vibrant",
         {QColor(255, 204, 204), QColor(255, 102, 102), QColor(255, 153, 153), Qt::gray,Qt::black}},
        {"Ocean",
         {QColor(204, 229, 255), QColor(102, 178, 255), QColor(153, 204, 255), Qt::gray,Qt::black}}
    };

    // 默认主题
    m_currentTheme = m_themes["Classic"];
}

QStringList ColorGroup::availableThemes() const {
    return m_themes.keys();
}

ColorTheme ColorGroup::getTheme(const QString& themeName) const {
    return m_themes.value(themeName, m_currentTheme);
}

void ColorGroup::setCurrentTheme(const QString& themeName) {
    if (m_themes.contains(themeName)) {
        m_currentTheme = m_themes[themeName];
    }
}

const ColorTheme& ColorGroup::currentTheme() const {
    return m_currentTheme;
}
ColorGroup& ColorGroup::instance() {
    static ColorGroup group;
    return group;
}

const ColorTheme& ColorGroup::current() {
    return instance().currentTheme();
}
