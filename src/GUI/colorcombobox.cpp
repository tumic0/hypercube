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

	insertItem(colorCount(), pix, name, color);
}

int ColorComboBox::colorCount() const
{
    return QComboBox::count() - 1;
}

QColor ColorComboBox::color() const
{
	return itemData(currentIndex()).value<QColor>();
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
	  QSize(fm.horizontalAdvance("#000000") + fm.height() + 4, fm.height()), this);
}

void ColorComboBox::emitActivatedColor(int index)
{
	QColor color;
	int currentIndex;

	if (index == colorCount()) {
		color = QColorDialog::getColor();

		if (color.isValid()) {
			if ((currentIndex = findData(color)) < 0) {
				addColor(color, color.name());
				setCurrentIndex(index);
			} else {
				setCurrentIndex(currentIndex);
			}
		} else {
			setColor(lastActivated);
			color = lastActivated;
		}
	} else
		color = itemData(index).value<QColor>();

	update();
	lastActivated = color;
	emit activated(color);
}
