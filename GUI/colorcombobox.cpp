#include <QColorDialog>
#include <QPainter>
#include "colorcombobox.h"

/*!
	\class ColorComboBox
	\brief A selection widget for choosing colors.

	The ColorComboBox widget provides a set of predefined colors and a special
	item that lets the user choose his own color using the QColorDialog.
*/


/*!
	\fn void ColorComboBox::activated(const QColor &color)

	This signal is emitted whenever a new color is choosen from the list.
	The \a color argument is the color of the newly selected item.

	The signal is not emitted when the current color is changed
	programmatically by calling setColor().
*/


/*!
	Constructs a color combo box widget.
*/
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

/*!
	Adds an item with the given \a color and \a name to the widget.
*/
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

/*!
	The current number of items in the widget.

	The special item for selecting user defined colors is not counted, use
	count() if you want the number of all items.
*/
int ColorComboBox::colorCount() const
{
    return QComboBox::count() - 1;
}

/*!
	The color of the currently selected item.
*/
QColor ColorComboBox::color() const
{
	return qVariantValue<QColor>(itemData(currentIndex()));
}

/*!
	Sets the widget's item with color \a color to be the current item.

	If there is no item with color \a color in the widget, the color is
	created.
*/
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

/*!
	\reimp
*/
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
