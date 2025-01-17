#ifndef REGEX_UTILS_H
#define REGEX_UTILS_H

#include <QRegularExpression>
/**
 * RegexUtils is used to store different regular expressions across the whole application.
 */
namespace RegexUtils {
    const QRegularExpression componentRegex = QRegularExpression(R"(\(component.+?\n\s\))", QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression nameRegex = QRegularExpression(R"(\(name \"(.+?)\"\))");
    const QRegularExpression paramRegex = QRegularExpression(R"((\d*[\.,]?\d*)(\w*))");
    const QRegularExpression attributeRegex = QRegularExpression(R"(\(attribute \"(\w+)\" \(type \w+\) (\(unit (\w+)\)) (\(value \"(.*)\"\))\))");
    const QRegularExpression signalRegex = QRegularExpression(R"(\(signal .+? \(net .+?\)\))");
    const QRegularExpression sourceTypesRegex = QRegularExpression(R"(DC|PWL|PULSE|SIN|CUS|NOISE|EXP|jj)", QRegularExpression::CaseInsensitiveOption);
    const QRegularExpression subcircuitRegex = QRegularExpression(R"(\**.*\.SUBCKT (\w+) .+?\.ENDS\n*)", QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression subcircuitIdentifierRegex = QRegularExpression(R"(^X\w+ (\w+))", QRegularExpression::MultilineOption);
    const QRegularExpression sourceTypeRegex = QRegularExpression(R"({{\w+/(\w+)}})");
    const QRegularExpression specialDeclaration = QRegularExpression(R"(\.\w+)");
    const QRegularExpression comment = QRegularExpression(R"(\s*\*.+)");
    const QRegularExpression paramWithName = QRegularExpression(R"((\w+)=(\w+))");
    const QRegularExpression projectPath = QRegularExpression(R"(.+[\\|/](\w+)[\\|/]circuit[\\|/]circuit\.lp)");
    const QRegularExpression jjNameRegex = QRegularExpression(R"(JJ([a-zA-Z0-9_]+))");
    const QRegularExpression jjSpiceNameRegex = QRegularExpression(R"(B([a-zA-Z0-9_]+))");
    const QRegularExpression jjSpiceRow = QRegularExpression(R"(<br>\s*B[a-zA-Z0-9_]+ \d+ \d+ ([a-zA-Z0-9_]+)\s*)");
    const QRegularExpression settingRegex = QRegularExpression(R"((\w+)=(.+))");
};

#endif // REGEX_UTILS_H
