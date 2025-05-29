#ifndef COLORGROUP_H
#define COLORGROUP_H

#include <QColor>
#include <QMap>
#include <QString>
#include <QPen>
#include <QBrush>

// 颜色主题结构体
struct ColorTheme {
    QColor neuronFill;
    QColor neuronBorder;
    QColor connectionHighWeight;  // 高权重连接线颜色
    QColor connectionLowWeight;   // 低权重连接线颜色
    QColor text;
};
class ColorGroup {
public:
    ColorGroup();
    // 获取所有可用主题名称
    QStringList availableThemes() const;
    // 获取指定主题的颜色配置
    ColorTheme getTheme(const QString& themeName) const;
    // 设置当前主题
    void setCurrentTheme(const QString& themeName);
    // 获取当前主题
    const ColorTheme& currentTheme() const;
    // 添加静态方法获取单例实例
    static ColorGroup& instance();

    // 添加静态方法获取当前主题
    static const ColorTheme& current();

private:
    QMap<QString, ColorTheme> m_themes;
    ColorTheme m_currentTheme;
};

#endif // COLORGROUP_H
