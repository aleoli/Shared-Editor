#pragma once

#include <QJsonObject>

class IdentifierBase {
public:
    IdentifierBase();
    IdentifierBase(int digit, int userId);
    explicit IdentifierBase(const QJsonObject &json);
    explicit IdentifierBase(QJsonObject &&json);
    IdentifierBase(const IdentifierBase&) = default;
    IdentifierBase(IdentifierBase &&) = default;

    IdentifierBase& operator=(const IdentifierBase& identifier) = default;
    IdentifierBase& operator=(IdentifierBase&& identifier) noexcept = default;

    static IdentifierBase fromJsonObject(const QJsonObject &json);
    static IdentifierBase fromJsonObject(QJsonObject &&json);
    [[nodiscard]] QJsonObject toJsonObject() const;

    friend bool operator==(const IdentifierBase& lhs, const IdentifierBase& rhs);
    friend bool operator<(const IdentifierBase& lhs, const IdentifierBase& rhs);

    [[nodiscard]] int getDigit() const;
    [[nodiscard]] int getUserId() const;
    [[nodiscard]] std::string to_string() const;

private:
    void checkAndAssign(const QJsonObject &json);

    int _digit{}, _userId{};
};
