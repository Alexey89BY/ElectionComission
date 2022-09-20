#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QClipboard>


static const QChar constMembersSeparator = '\n';
static const QChar constSpaceSeparator = ' ';
static const QString constStringDistrictLotMark = QStringLiteral("\t%DL%\t");


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  setWindowTitle(QStringLiteral("Election Comission @ %1 %2").arg(__DATE__).arg(__TIME__));
}


MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::on_btnPreFormat_clicked()
{
  if (ui->checkSplitMode->isChecked())
  {
    preFormatSplitted();
  }
  else
  {
    preFormatJoined();
  }
}


void MainWindow::preFormatSplitted()
{
  QStringList listNames;

  {
    QString textNames = ui->editMembersA1->toPlainText();
    textNames = textNames.simplified();

    int index = 0;
    int indexPrev = 0;
    int spacesNumber = 1;

    while (index >= 0)
    {
      index = textNames.indexOf(constSpaceSeparator, index);

      if (
          (index >= 0)
       && (spacesNumber < ui->spinSkipSpaces->value())
         )
      {
        ++spacesNumber;
      }
      else
      {
        QString name = textNames.mid(indexPrev, (index < 0)? (-1): (index-indexPrev));
        name = name.simplified();
        listNames.append(name);

        spacesNumber = 1;
        indexPrev = index;
      }

      if (index >= 0)
      {
        ++index;
      }
    }
  }

  QStringList listInfos;

  {
    QString textInfos = ui->editMembersA2->toPlainText();
    textInfos = textInfos.simplified();

    QString separatorText = ui->editMetaChar->text();
    QString markString = constStringDistrictLotMark + ui->editMetaCharReplace->text();

    int index = 0;
    int indexPrev = -1;

    while (index >= 0)
    {
      index = textInfos.indexOf(separatorText, index);

      if (indexPrev >= 0)
      {
        QString info = textInfos.mid(indexPrev, (index < 0)? (-1): (index-indexPrev));
        info = info.simplified();
        info.replace(0, separatorText.length(), markString);
        listInfos.append(info);
      }

      if (index >= 0)
      {
        indexPrev = index;
        index += separatorText.length();
      }
    }
  }

  ui->editFormatStats->setText(QStringLiteral("%1 / %2").arg(listNames.count()).arg(listInfos.count()));

  int linesCount = std::min(listNames.count(), listInfos.count());
  QStringList resultList;

  for (int index =0; index < linesCount; ++index)
  {
    resultList.append(listNames.at(index) + listInfos.at(index));
  }

  ui->editMembersB->setPlainText(resultList.join(constMembersSeparator));
}


void MainWindow::preFormatJoined()
{
  QString text = ui->editMembersA1->toPlainText();
  text.append(constSpaceSeparator);
  text.append(ui->editMembersA2->toPlainText());
  text = text.simplified();

  int linesCount = 1;
  QString separatorText = ui->editMetaChar->text();

  if (! separatorText.isEmpty())
  {
    int separatorIndex = 0;
    QString markString = constStringDistrictLotMark + ui->editMetaCharReplace->text();

    while (separatorIndex >= 0)
    {
      separatorIndex = text.indexOf(separatorText, separatorIndex);
      if (separatorIndex < 0)
      {
        break;
      }

      int spacesNumber = ui->spinSkipSpaces->value();
      int charIndex = separatorIndex-1;

      while (charIndex >= 0)
      {
        if (text.at(charIndex) == constSpaceSeparator)
        {
          if (spacesNumber <= 0)
          {
            break;
          }

          --spacesNumber;
        }

        --charIndex;
      }

      ++charIndex;

      if (
          text.at(charIndex).isUpper()
       || text.at(charIndex).isDigit()
         )
      {
        if (charIndex > 0)
        {
          text.replace(charIndex - 1, 1, constMembersSeparator);
          ++linesCount;
        }

        if (ui->checkRemovePrefix->isChecked())
        {
          int prefixEndIndex = text.indexOf(constSpaceSeparator, charIndex);
          int prefixLength = (prefixEndIndex+1) - charIndex;

          text.remove(charIndex, prefixLength);
          separatorIndex -= prefixLength;
        }

        if (ui->spinCutLast->value() > 0)
        {
          int cutLength = ui->spinCutLast->value();
          if (charIndex > cutLength)
          {
            text.remove(charIndex - 1 - cutLength, cutLength);
            separatorIndex -= cutLength;
          }
        }

        text.replace(separatorIndex, separatorText.length(), markString);
        separatorIndex += markString.length();
      }
      else
      {
        ++separatorIndex;
      }
    }

    if (ui->spinCutLast->value() > 0)
    {
      int cutLength = ui->spinCutLast->value();
      if (text.length() > cutLength - 1)
      {
        text.remove(text.length() - cutLength, cutLength);
      }
    }
  }

  ui->editFormatStats->setText(QStringLiteral("%1").arg(linesCount));
  ui->editMembersB->setPlainText(text);
}


void MainWindow::on_btnCopyToClipboard_clicked()
{
  QString plainText = ui->editMembersC->toPlainText();
  QApplication::clipboard()->setText(plainText);
}


void MainWindow::on_btnGenerateReport_clicked()
{
  QStringList membersList = ui->editMembersB->toPlainText().split(constMembersSeparator, QString::SplitBehavior::SkipEmptyParts);
  QStringList directorsList = ui->editDirectors->toPlainText().split(constMembersSeparator);
  QStringList resultList;

  for (auto const &directorName: directorsList)
  {
    QString memberPrefix = directorName.simplified();
    if (memberPrefix.isEmpty())
    {
      continue;
    }

    int memberIndex = 0;

    for (auto const &memberName: membersList)
    {
      if (memberName.startsWith(memberPrefix, Qt::CaseSensitivity::CaseInsensitive))
      {
        break;
      }

      ++memberIndex;
    }

    if (memberIndex < membersList.count())
    {
      if (ui->checkSortMembers->isChecked())
      {
        resultList.append(membersList.at(memberIndex));
        membersList.removeAt(memberIndex);
      }
    }
    else
    {
      resultList.append(QStringLiteral("!!! ERROR - NOT FOUND '%1' !!!").arg(memberPrefix));
    }
  }

  resultList.append(membersList);

  QString regionInfo = (ui->spinLot->value() != 0)
      ? (QStringLiteral("\t%1%2%3\t").arg(ui->spinDistrict->value()).arg(ui->editReportChar->text()).arg(ui->spinLot->value()))
      : (QStringLiteral("\t%1\t").arg(ui->spinDistrict->value()));

  for (auto &memberName: resultList)
  {
    memberName.replace(constStringDistrictLotMark, regionInfo);
  }

  QString districtAddress = ui->editAddress->toPlainText().simplified();

  if (districtAddress.length() > 0)
  {
    districtAddress.append('\t');

    for (auto &memberName: resultList)
    {
      memberName.prepend(districtAddress);
    }
  }

  ui->editReportStats->setText(QStringLiteral("%1").arg(resultList.count()));
  ui->editMembersC->setPlainText(resultList.join(constMembersSeparator));
}


void MainWindow::on_btnNextLot_clicked()
{
  ui->spinLot->setValue(ui->spinLot->value() + 1);
  ui->editDirectors->clear();
  ui->editAddress->clear();
  ui->editMembersA1->clear();
  ui->editMembersA2->clear();
  ui->editMembersB->clear();
  ui->editFormatStats->clear();
  ui->editMembersC->clear();
  ui->editReportStats->clear();
}


