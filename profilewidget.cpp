#include "profilewidget.h"
#include "ui_profilewidget.h"
#include <QGraphicsDropShadowEffect>
#include <QScrollArea>
#include <QDebug>
#include <QPushButton>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

ProfileWidget::ProfileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfileWidget)
{
    ui->setupUi(this);
    ui->actionsFrame->hide();

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(10);
    shadow->setOffset(0.2, 0.5);
    ui->pictureLabel->setGraphicsEffect(shadow);
}

ProfileWidget::~ProfileWidget()
{
    delete ui;
}

void ProfileWidget::setPrintFileName(const QString &printFileName)
{
    m_printFileName = printFileName;
}

void ProfileWidget::setProfilePicture(const QString &filePath)
{
    QPixmap pixmap(filePath);
    if(!pixmap.isNull())
    {
        m_pictureFilePath = filePath;
        ui->pictureLabel->setPixmap(pixmap);
    }
}

void ProfileWidget::addSection(const QString &tabTitle,
                               const QString &sectionTitle,
                               const QString &customHtml)
{
    QWidget *widget = nullptr;
    for (int i = 0; i < ui->tabWidget->count(); ++i)
    {
        if(ui->tabWidget->tabText(i) == tabTitle)
        {
            widget = ui->tabWidget->widget(i);
            break;
        }
    }

    if(!widget)
    {
        widget = new QWidget;
        QGridLayout *gridLayout = new QGridLayout;
        widget->setLayout(gridLayout);

        QScrollArea *scrollArea = new QScrollArea;
        scrollArea->setWidgetResizable(true);
        gridLayout->addWidget(scrollArea);

        QWidget *scrollAreaWidget = new QWidget;
        scrollArea->setWidget(scrollAreaWidget);

        QVBoxLayout *scrollAreaLayout = new QVBoxLayout;
        scrollAreaWidget->setLayout(scrollAreaLayout);

        QLabel *sectionLabel = new QLabel;
        sectionLabel->setObjectName("sectionLabel");
        scrollAreaLayout->addWidget(sectionLabel);
        scrollAreaLayout->setAlignment(sectionLabel, Qt::AlignTop);

        sectionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        sectionLabel->setOpenExternalLinks(true);
        sectionLabel->setMaximumWidth(800);
        sectionLabel->setWordWrap(true);

        ui->tabWidget->addTab(widget, tabTitle);
    }

    QLabel *sectionLabel = widget->findChild<QLabel*>("sectionLabel");
    if(sectionLabel)
    {
        QString text = sectionLabel->text();
        if(!text.isEmpty())
            text += "<hr>";

        text += QString(R"(
                        <div dir='rtl'>
                        <div></div><h4>%0</h4>
                        )").arg(sectionTitle);

        text += customHtml;
        text += "</div>";

        sectionLabel->setText(text);
    }
}

void ProfileWidget::addSection(const QString &tabTitle,
                               const QString &sectionTitle,
                               const QStringList &data)
{
    QString text = QString(R"(
                           <table align='right' cellspacing='5'>
                           <tbody>
                           )");

    foreach (const QString &d, data)
    {
        if(d.contains("|"))
        {
            QString s1 = d.section("|", 0,0) + ":";
            QString s2 = d.section("|", 1,1);

            text += QString(R"(
                            <tr>
                            <td >%1</td>
                            <td width='150'>%0</td>
                            </tr>
                            )")
                    .arg(s1)
                    .arg(s2);
        }
        else
        {
            text += QString(R"(
                            <tr>
                            <td>%0</td>
                            <td width='150'>:</td>
                            </tr>
                            )")
                    .arg(d);
        }
    }

    text += QString(R"(
                    </tbody>
                    </table>
                    )");

    addSection(tabTitle, sectionTitle, text);
}

void ProfileWidget::addTableSection(const QString &tabTitle,
                                    const QString &sectionTitle,
                                    const QStringList &headers,
                                    const QList<QStringList> &data)
{
    QString text = R"(
                   <table align='right' border='1' cellspacing='0' cellpadding='6'>
                   <thead>
                   <tr>
                   )";

    for (int i = 0; i < headers.size(); ++i)
    {
        text += "<th align='center' bgcolor='lightgray'>"+headers.at(i)+"</th>";
    }
    text += R"(
            </tr>
            </thead>
            <tbody>
            )";

    for (int row = 0; row < data.size(); ++row)
    {
        QStringList rowData = data.at(row);
        text += "<tr>";
        for (int i = 0; i < rowData.count(); ++i)
        {
            text += "<td align='center'>"+rowData.at(i)+"</td>";
        }
        text += "</tr>";
    }

    text += QString(R"(
                    </tbody>
                    </table>
                    )");

    addSection(tabTitle, sectionTitle, text);
}

void ProfileWidget::addTableSectionFromSQLQuery(const QString &tabTitle,
                                                const QString &sectionTitle,
                                                const QString &sqlQuery)
{
    QSqlQuery query;
    query.exec(sqlQuery);
    if(query.lastError().isValid())
    {
        qDebug().noquote() << query.lastError().text();
        return;
    }

    if(query.size() > 0)
    {
        QLocale egyptian(QLocale::Arabic, QLocale::Egypt);

        QStringList headers;
        QSqlRecord record = query.record();
        for (int i = record.count()-1; i >= 0; --i)
            headers << record.fieldName(i);

        QList<QStringList> data;
        while (query.next())
        {
            QStringList rowData;
            for (int i = record.count()-1; i >= 0; --i)
            {
                QString value = query.value(i).toString();
                if(QDate::fromString(value, "yyyy-MM-dd").isValid())
                    value = egyptian.toString(QDate::fromString(value, "yyyy-MM-dd"), "yyyy/M/d");

                rowData << value;
            }
            data << rowData;
        }
        addTableSection(tabTitle,sectionTitle, headers, data);
    }
}

void ProfileWidget::addActionButton(const QString &text,
                              const QString &icon,
                              const std::function<void()> &action)
{
    if(ui->actionsFrame->isHidden())
        ui->actionsFrame->show();

    QPushButton *actionBtn = new QPushButton(QIcon(icon), text);
    ui->actionsFrameVerticalLayout->addWidget(actionBtn);
    actionBtn->setCursor(Qt::PointingHandCursor);
    actionBtn->setIconSize(QSize(24, 24));

    actionBtn->setStyleSheet(R"(
                             QPushButton{
                             border: 1px solid lightgray;
                             border-radius: 2px;
                             background-color: rgb(230, 230, 230);
                             padding: 2px;
                             }
                             QPushButton:hover{
                             background-color: rgb(240, 240, 240);
                             }
                             )");
    actionBtn->setFlat(true);
    connect(actionBtn, &QPushButton::clicked, action);
}

void ProfileWidget::addSubPisctureDetails(const QString &data)
{
    QLabel *label = new QLabel(data);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    label->setOpenExternalLinks(true);
    label->setWordWrap(true);
    ui->subImageDetailsVerticalLayout->addWidget(label);
}

//
#include <QTextDocument>
#include <QPrinter>
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <QFileDialog>
#include <QDesktopServices>

//https://stackoverflow.com/questions/44537788/page-x-of-y-using-qprinter/44540195#44540195
static const int textMargins = 12; // in millimeters
static const int borderMargins = 10; // in millimeters

static double mmToPixels(QPrinter& printer, int mm)
{
    return mm * 0.039370147 * printer.resolution();
}

static void paintPage(int pageNumber, int pageCount,
                      QPainter* painter, QTextDocument* doc,
                      const QRectF& textRect, qreal footerHeight)
{
    painter->save();
    // textPageRect is the rectangle in the coordinate system of the QTextDocument, in pixels,
    // and starting at (0,0) for the first page. Second page is at y=doc->pageSize().height().
    const QRectF textPageRect(0, pageNumber * doc->pageSize().height(), doc->pageSize().width(), doc->pageSize().height());
    // Clip the drawing so that the text of the other pages doesn't appear in the margins
    painter->setClipRect(textRect);
    // Translate so that 0,0 is now the page corner
    painter->translate(0, -textPageRect.top());
    // Translate so that 0,0 is the text rect corner
    painter->translate(textRect.left(), textRect.top());
    doc->drawContents(painter);
    painter->restore();
    QRectF footerRect = textRect;
    footerRect.setTop(textRect.bottom());
    footerRect.setHeight(footerHeight);

    QFont font = painter->font();
    font.setPointSize(font.pointSize()-2);
    painter->setFont(font);
    painter->drawText(footerRect, Qt::AlignCenter, QObject::tr("%1 - %2")
                      .arg(pageNumber+1).arg(pageCount));


    QString printingTime = QDateTime::currentDateTime().toString("dddd yyyy/M/d h:mm AP");
    painter->drawText(footerRect, Qt::AlignLeft, printingTime);
}

static void printDocument(QPrinter& printer, QTextDocument* doc)
{
    QPainter painter( &printer );
    doc->documentLayout()->setPaintDevice(&printer);
    doc->setPageSize(printer.pageRect().size());
    QSizeF pageSize = printer.pageRect().size(); // page size in pixels
    // Calculate the rectangle where to lay out the text
    const double tm = mmToPixels(printer, textMargins);
    const qreal footerHeight = painter.fontMetrics().height();
    const QRectF textRect(tm, tm, pageSize.width() - 2 * tm, pageSize.height() - 2 * tm - footerHeight);
    doc->setPageSize(textRect.size());

    const int pageCount = doc->pageCount();

    bool firstPage = true;
    for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex) {

        if (!firstPage)
            printer.newPage();

        paintPage(pageIndex, pageCount, &painter, doc, textRect, footerHeight );
        firstPage = false;
    }
}

void ProfileWidget::on_printBtn_clicked()
{
    if(ui->tabWidget->count() == 0)
        return;

    QString printedText;
    for (int i = 0; i < ui->tabWidget->count(); ++i)
    {
        QWidget *widget = ui->tabWidget->widget(i);
        QLabel *sectionLabel = widget->findChild<QLabel*>("sectionLabel");
        if(sectionLabel)
        {
            QString text = sectionLabel->text();
            if(text.isEmpty())
                continue;

            printedText += text;
        }
    }

    if(printedText.isEmpty())
        return;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save PDF File"),
                                 m_printFileName, tr("PDF (*.pdf)"));

    if(fileName.isEmpty())
        return;

    if(!m_pictureFilePath.isEmpty())
    {
        QString image = QString(R"(
                                <div align='right'>
                                <img src='%0' width='180'/>
                                </div>
                                )")
                .arg(m_pictureFilePath);
        printedText = image + printedText;
    }

    QTextDocument document;
    document.setHtml(printedText);

    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
    printer.setOutputFormat(QPrinter::PdfFormat);

    printer.setOutputFileName(fileName);
    printDocument(printer, &document);

    if(QFile::exists(fileName))
        QDesktopServices::openUrl(QUrl::fromUserInput(fileName));
}
