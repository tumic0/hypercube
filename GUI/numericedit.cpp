#include <cfloat>
#include <climits>
#include "numericedit.h"


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

void FloatEdit::setValue(float value)
{
	_value = value;
	setText(QString("%1").arg(value));
	emit valueChanged(_value);
}

void FloatEdit::emitEditedValue(const QString &text)
{
	_value = text.toFloat();
	emit valueChanged(_value);
}


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

void IntEdit::setValue(int value)
{
	_value = value;
	setText(QString("%1").arg(value));
	emit valueChanged(_value);
}

void IntEdit::emitEditedValue(const QString &text)
{
	_value = text.toInt();
	emit valueChanged(_value);
}
