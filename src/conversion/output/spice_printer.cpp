#include "spice_printer.h"
#include "src/utils/attribute_utils.h"
#include "src/utils/global_variables.h"

Q_GLOBAL_STATIC(QString, EMPTY_STRING, QString());
Q_GLOBAL_STATIC(QString, LINE_SEPARATOR, QString("<br>"));
Q_GLOBAL_STATIC(QString, WORD_SEPARATOR, QString(" "));
Q_GLOBAL_STATIC(QString, MODEL, QString(".MODEL %1 %2"));
Q_GLOBAL_STATIC(QString, TRAN, QString(".tran %1"));
Q_GLOBAL_STATIC(QString, PROBE, QString(".PRINT %1 %2 0"));
Q_GLOBAL_STATIC(QString, METER, QString(".PRINT %1 %2"));
Q_GLOBAL_STATIC(QString, NODEV, QString(".PRINT NODEV %1 %2"));
Q_GLOBAL_STATIC(QString, NODEP, QString(".PRINT NODEP %1 %2"));
Q_GLOBAL_STATIC(QString, SUBCIRCUIT, QString("X%1 %2"));
Q_GLOBAL_STATIC(QString, FILE_OUTPUT, QString(".FILE %1"));
Q_GLOBAL_STATIC(QString, CURRENT_WARNING,
    QString("<br><b style=\"color:orange\">*WARNING! There are multiple devices connected to the ammeter %1. The first "
            "connected device was chosen. Specify the device name in the %1 parameters.<br>%2</b><br>"));
Q_GLOBAL_STATIC(QString, CURRENT_ERROR,
                QString("<br><b style=\"color:red\">*ERROR! Ammeter %1 was misplaced. Cannot display of output.</b><br>"));

SpicePrinter::SpicePrinter(const QMap<QString, QString> &netLabelMap,
                           const QMap<QString, QSet<Component>> &netComponentsMap,
                           const ConversionParams &params)
    : netLabelMap(netLabelMap)
    , netComponentsMap(netComponentsMap)
    , params(params)
{}

SpicePrinter::~SpicePrinter() {}

QString writeSignal(QString uuid, QMap<QString, QString> netLabelMap)
{
    return uuid.isEmpty() || netLabelMap.isEmpty() ? "_ " : netLabelMap[uuid];
}

QString SpicePrinter::printComponent(Component component, QString parentUUID)
{
    QString result = component.getName() + *WORD_SEPARATOR;
    QList<Attribute> list = component.getAttributeList();
    if (component.getValue() == "{{SUBCIRCUIT}}") {
        result = SUBCIRCUIT->arg(list.first().getValue().toUpper(), result);
        component.removeAttribute(list.first());
    }
    if (!parentUUID.isEmpty()) {
        result += writeSignal(parentUUID, netLabelMap) + *WORD_SEPARATOR;
    }
    for (Signal &signal : component.getSignalList()) {
        if (signal.getNet().getUuid() != parentUUID) {
            result += writeSignal(signal.getNet().getUuid(), netLabelMap) + *WORD_SEPARATOR;
        }
    }
    if (component.getValue() == "VCC") {
        result += "0 ";
    }
    result += attributeUtils::writeAttributes(component.getAttributeList(), component.getValue());
    return result;
}

QString SpicePrinter::printModel(Component model)
{
    QList<Attribute> attributes = model.getAttributeList();
    if(params.getSimulatorVersion() == SIMULATOR_VERSION_JSIM) {
        auto condition = [](Attribute attribute) { return !JSIM_MODEL_ATTRIBUTES.contains(attribute.getName()); };
        attributes.erase(std::remove_if(attributes.begin(), attributes.end(), condition));
    }
    return MODEL->arg(model.getName(), attributeUtils::writeAttributes(attributes, model.getValue(), true));
}

QString printTran(Component tran)
{
    for (Attribute &attribute : tran.getAttributeList()) {
        if (attribute.getName() == "ANALYSIS") {
            tran.removeAttribute(attribute);
            break;
        }
    }
    return TRAN->arg(attributeUtils::writeAttributes(tran.getAttributeList()));
}

QString SpicePrinter::print(Component component, QString parentUUID)
{
    QString elementType = component.getElementType();
    if (elementType == "component") {
        return printComponent(component, parentUUID);
    }
    if (elementType == "model") {
        return printModel(component);
    }
    if (elementType == "tran") {
        return printTran(component);
    }
    return *EMPTY_STRING;
}

QString SpicePrinter::printProbe(Component component) {
    Attribute printType;
    for (Attribute &attribute : component.getAttributeList()) {
        if (attribute.getName() == "PRINT_TYPE") {
            printType = attribute;
        }
    }
    QString signal = writeSignal(component.getSignalList().constFirst().getNet().getUuid(), netLabelMap);
    return PROBE->arg(printType.getValue(), signal);
}


QString* getNodevMode(Attribute printType)
{
    if (printType.getValue() == "PHASE") {
        return NODEP;
    }
    return NODEV;
}

QString printNodev(Attribute printType, QString firstSignal, QString secondSignal) {
    return getNodevMode(printType)->arg(firstSignal, secondSignal);
}

QString SpicePrinter::printMeter(Component component)
{
    QString firstUuid = component.getSignalList().constFirst().getNet().getUuid();
    QString secondUuid = component.getSignalList().constLast().getNet().getUuid();
    QSet<Component> first = netComponentsMap.value(firstUuid);
    QSet<Component> second = netComponentsMap.value(secondUuid);
    Attribute device, printType;
    QString mode;

    for (Attribute &attribute : component.getAttributeList()) {
        if (attribute.getName() == "DEVICE_NAME") {
            device = attribute;
        }
        if (attribute.getName() == "PRINT_TYPE") {
            printType = attribute;
        }
        if (attribute.getName() == "CURRENT_MODE" && params.getSimulatorVersion() == 0) {
            mode = " " + attribute.getValue();
        }
    }

    QList<Component> intersection;
    for (const Component &component : first) {
        if (second.contains(component)) {
            intersection.append(component);
        }
    }

    if (intersection.empty()) {
        if (printType.getValue() == "DEVI") {
            return CURRENT_ERROR->arg(component.getName());
        }
        return printNodev(printType, netLabelMap[firstUuid], netLabelMap[secondUuid]);
    }

    if (device.getName().isEmpty() && intersection.size() == 1) {
        return METER->arg(printType.getValue(), intersection.first().getName() + mode);
    }

    auto condition = [&device](const Component &component) {
        return device.getValue() == component.getName();
    };
    bool deviceMatchesComponent = std::any_of(intersection.begin(), intersection.end(), condition);

    if (deviceMatchesComponent) {
        return METER->arg(printType.getValue(), device.getValue() + mode);
    }

    if (printType.getValue() == "DEVI") {
        QString firstDevice = METER->arg(printType.getValue(), intersection.first().getName() + mode);
        return CURRENT_WARNING->arg(component.getName(), firstDevice);
    }

    return printNodev(printType, netLabelMap[firstUuid], netLabelMap[secondUuid]);
}

QString SpicePrinter::printOutput(Component component) {
    if (component.getElementType() == "probe") {
        return printProbe(component);
    }
    if (component.getElementType() == "meter") {
        return printMeter(component);
    }
    return *EMPTY_STRING;
}

QString SpicePrinter::printOutputs(QList<Component> components)
{
    if (!params.getConsoleOutput() && !params.getFileOutput()) {
        return *EMPTY_STRING;
    }
    QMap<QString, QList<Component>> outputsMap;
    for (Component &component : components) {
        QString fileName = *EMPTY_STRING;
        for (Attribute &attribute : component.getAttributeList()) {
            if (attribute.getName() == "FILENAME") {
                fileName = attribute.getValue();
            }
        }
        QList<Component> assignedComponents = outputsMap.value(fileName);
        assignedComponents.append(component);
        outputsMap.insert(fileName, assignedComponents);
    }
    if (!params.getConsoleOutput()) {
        outputsMap.remove(*EMPTY_STRING);
    }
    QString result;
    QList<QString> fileNames = outputsMap.keys();
    fileNames.sort();
    for (QString &fileName : fileNames) {
        if (!fileName.isEmpty() && params.getFileOutput()) {
            result += FILE_OUTPUT->arg(fileName) + *LINE_SEPARATOR;
        }
        for (Component &component : outputsMap.value(fileName)) {
            result += printOutput(component) + *LINE_SEPARATOR;
        }
    }
    return result;
}
