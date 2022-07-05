#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>
#include <functional>

namespace Ui {
class ProfileWidget;
}

class ProfileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProfileWidget(QWidget *parent = 0);
    ~ProfileWidget();

    void setProfilePicture(const QString &filePath);

    ///
    /// \brief general implementation
    /// \param tabTitle
    /// \param sectionTitle
    /// \param customHtml
    ///
    void addSection(const QString &tabTitle,
                    const QString &sectionTitle,
                    const QString &customHtml);

    /////
    /// \brief add title and value
    /// \param tabTitle
    /// \param sectionTitle
    /// \param data e.g {"title|value", "name|John doe", "age|30"}
    ///
    void addSection(const QString &tabTitle,
                    const QString &sectionTitle,
                    const QStringList &data);

    ///
    /// \brief addTableSection
    /// \param tabTitle
    /// \param sectionTitle
    /// \param headers  - table header
    /// \param data - table body
    ///
    void addTableSection(const QString &tabTitle,
                         const QString &sectionTitle,
                         const QStringList &headers,
                         const QList<QStringList> &data);

    void addTableSectionFromSQLQuery(const QString &tabTitle,
                                const QString &sectionTitle,
                                const QString &sqlQuery);
    ///
    /// \brief add buttons under picture
    /// \param text
    /// \param icon
    /// \param action
    ///
    void addActionButton(const QString &text,
                   const QString &icon,
                   const std::function<void()> &action);

    void addSubPisctureDetails(const QString &data);

    //
    void setPrintFileName(const QString &printFileName);

private slots:
    void on_printBtn_clicked();

private:
    Ui::ProfileWidget *ui;

    QString m_pictureFilePath;
    QString m_printFileName;
};

#endif // PROFILEWIDGET_H
