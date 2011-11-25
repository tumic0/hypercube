#include <QColorDialog>
#include <QPainter>
#include "colorcombobox.h"


ColorComboBox::ColorComboBox(QWidget *parent)
	: QComboBox(parent)
{
	connect(this, SIGNAL(activated(int)), SLOT(emitActivatedColor(int)));

	addItem(tr("More..."));

	addColor(Qt::black, tr("Black"));
	addColor(Qt::red, tr("Red"));
	addColor(Qt::green, tr("Green"));
	addColor(Qt::blue, tr("Blue"));

	lastActivated = Qt::black;
	setColor(lastActivated);
}

void ColorComboBox::addColor(const QColor &color, const QString &name)
{
	QFontMetrics fm = fontMetrics();
	int iconSize = fm.height() - 2;
	QPixmap pix(iconSize, iconSize);
	QPainter painter(&pix);

	painter.setPen(Qt::gray);
	painter.setBrush(QBrush(color));
	painter.drawRect(0, 0, iconSize, iconSize);
	QIcon icon(pix);

	insertItem(colorCount(), pix, name, color);
}

int ColorComboBox::colorCount() const
{
    return QComboBox::count() - 1;
}

QColor ColorComboBox::color() const
{
	return qVariantValue<QColor>(itemData(currentIndex()));
}

void ColorComboBox::setColor(const QColor &color)
{
	int index;

	if ((index = findData(color)) >= 0)
		setCurrentIndex(index);
	else {
		addColor(color, color.name());
		setCurrentIndex(colorCount() - 1);
	}
}

QSize ColorComboBox::sizeHint() const
{
	QFontMetrics fm = fontMetrics();
	QStyleOptionComboBox box;
	box.initFrom(this);
	return style()->sizeFromContents(QStyle::CT_ComboBox, &box,
	  QSize(fm.width("#000000") + fm.height() + 4, fm.height()), this);
}

void ColorComboBox::emitActivatedColor(int index)
{
	int currentIndex;

	if (index == colorCount()) {
		QColor col = QColorDialog::getColor();

		if (col.isValid()) {
			if ((currentIndex = findData(col)) < 0) {
				addColor(col, col.name());
				setCurrentIndex(index);
			} else {
				setCurrentIndex(currentIndex);
			}
		} else {
			setColor(lastActivated);
			col = lastActivated;
		}

		update();
		lastActivated = col;
		emit activated(col);
	} else {
		QColor col = qVariantValue<QColor>(itemData(index));
		update();
		lastActivated = col;
		emit activated(col);
	}
}
