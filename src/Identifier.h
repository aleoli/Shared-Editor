#pragma once

#include <QJsonObject>
#include <QString>

class Identifier {
public:
    Identifier();
    Identifier(int first, int second);
    explicit Identifier(const QJsonObject &json);
    explicit Identifier(QJsonObject &&json);
    Identifier(const Identifier&) = default;
    Identifier(Identifier &&) = default;

    Identifier& operator=(const Identifier& identifier) = default;
    Identifier& operator=(Identifier&& identifier) noexcept = default;

    static Identifier fromJsonObject(const QJsonObject &json);
    static Identifier fromJsonObject(QJsonObject &&json);
    [[nodiscard]] QJsonObject toJsonObject() const;

    friend bool operator==(const Identifier& lhs, const Identifier& rhs);
    friend bool operator!=(const Identifier& lhs, const Identifier& rhs);
    friend bool operator<(const Identifier& lhs, const Identifier& rhs);

    [[nodiscard]] int getFirst() const;
    [[nodiscard]] int getSecond() const;
    [[nodiscard]] std::string to_string() const;
    [[nodiscard]] QString toString() const;

private:
    void checkAndAssign(const QJsonObject &json);

    int _first, _second;
};
