#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    if (argc != 2) {
        qDebug() << "Uso: ConvertirKML archivo.kml";
        return 1;
    }

    QString kmlFileName = argv[1];

    QFile kmlFile(kmlFileName);

    if (!kmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "No se pudo abrir el archivo KML";
        return 1;
    }

    QDomDocument doc;
    if (!doc.setContent(&kmlFile)) {
        qDebug() << "No se pudo analizar el archivo KML";
        kmlFile.close();
        return 1;
    }
    kmlFile.close();

    // Obtener la lista de marcadores
    QDomNodeList placemarkers = doc.elementsByTagName("Placemark");

    // Abrir el archivo TXT y escribir el encabezado
    QFile txtFile("uncover3D_dictionary.txt");
    if (!txtFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "No se pudo abrir el archivo TXT";
        return 1;
    }

    QTextStream txtStream(&txtFile);
    txtStream << "__uncover3D_info = { ";
    int count = 0;

    for (int i = 0; i < placemarkers.count(); ++i) {
        QDomElement placemarker = placemarkers.at(i).toElement();
        QString name = placemarker.firstChildElement("name").text();
        QString coordinates = placemarker.firstChildElement("Point")
                                  .firstChildElement("coordinates").text();
        QString lat = coordinates.split(',').at(1);
        QString lon = coordinates.split(',').at(0);
        QString type = name.split("_").at(1);

        txtStream << i << ": {'lat': " << lat << ", "
                  << "'lon': " << lon << ", "
                  << "'event': '" << name << "', "
                  << "'value_can': " << type << ", "
                  << "'dist_to_next': 0, "
                  << "'current_dist': 0}";

        if (i == placemarkers.count() - 1) {
            txtStream << " }\n\n";
        } else {
            txtStream << ",\n                  ";
        }

        count++;
    }

    txtFile.close();
    qDebug() << "Puntos obtenidos: " << count;

    return 0;
}
