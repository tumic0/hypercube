#include <cfloat>
#include <climits>
#include "numericedit.h"


/*!
	\class FloatEdit
	\brief A text box widget for editing floating point numbers.

	FloatEdit is a modified text edit widget that accepts only positive
	floating point numbers as contents.
*/


/*!
	\fn void FloatEdit::valueChanged(float value)

	This signal is emitted whenever the text is edited. The \a value argument
	is the new value.

	The signal is not emitted when the text is changed programmatically by
	calling setValue().
*/


/*!
	Constructs a float edit large enough to display numbers with
	\a digits digits.

	The widget has an empty text field and a value of 0 upon creation.
*/
FloatEdit::FloatEdit(int digits, QWidget *parent)
	: QLineEdit(parent)
{
	setValidator(new QDoubleValidator(0, FLT_MAX, FLT_DIG, this));

	int borders = minimumSizeHint().width() - fontMetrics().maxWidth();
	setMaximumWidth(fontMetrics().width('0') * digits + borders);

	connect(this, SIGNAL(textEdited(const QString&)),
	  this, SLOT(emitEditedValue(const QString&)));

	_value = 0;
}

/*!
	The current numeric value of the widget.
*/
float FloatEdit::value(void) {
	return _value;
}

/*!
	Sets the value of the widget to \a value.

	Setting this property updates the widget's numeric value and makes the
	widget display a corresponding text.
*/
void FloatEdit::setValue(float value)
{
	_value = value;
	setText(QString("%1").arg(value));
}

void FloatEdit::emitEditedValue(const QString &text)
{
	_value = text.toFloat();
	emit valueChanged(_value);
}


/*!
	\class IntEdit
	\brief A text box widget for editing integer numbers.

	IntEdit is a modified text edit widget that accepts only positive
	integer numbers as contents.
*/


/*!
	\fn void IntEdit::valueChanged(int value)

	This signal is emitted whenever the text is edited. The \a value argument
	is the new value.

	The signal is not emitted when the text is changed programmatically by
	calling setValue().
*/


/*!
	Constructs an int edit large enough to display numbers with
	\a digits digits.

	The widget has an empty text field and a value of 0 upon creation.
*/
IntEdit::IntEdit(int digits, QWidget *parent)
	: QLineEdit(parent)
{
	setValidator(new QIntValidator(0, INT_MAX, this));

	int borders = minimumSizeHint().width() - fontMetrics().maxWidth();
	setMaximumWidth(fontMetrics().width('0') * digits + borders);

	connect(this, SIGNAL(textEdited(const QString&)),
	  this, SLOT(emitEditedValue(const QString&)));

	_value = 0;
}

/*!
	The current numeric value of the widget.
*/
int IntEdit::value(void) {
	return _value;
}

/*!
	Sets the value of the widget to \a value.

	Setting this property updates the widget's numeric value and makes the
	widget display a corresponding text.
*/
void IntEdit::setValue(int value)
{
	_value = value;
	setText(QString("%1").arg(value));
}

void IntEdit::emitEditedValue(const QString &text)
{
	_value = text.toInt();
	emit valueChanged(_value);
}
