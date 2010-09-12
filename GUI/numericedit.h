#ifndef NUMERICEDIT_H_
#define NUMERICEDIT_H_

#include <QtGui>

class FloatEdit : public QLineEdit
{
	Q_OBJECT

public:
	FloatEdit(int digits, QWidget *parent = 0);
	float value(void);
	void setValue(float value);

signals:
	void valueChanged(float);

private slots:
	void emitEditedValue(const QString &text);

private:
	float _value;
};


class IntEdit : public QLineEdit
{
	Q_OBJECT

public:
	IntEdit(int digits, QWidget *parent = 0);
	int value(void);
	void setValue(int value);

signals:
	void valueChanged(int);

private slots:
	void emitEditedValue(const QString &text);

private:
	int _value;
};

#endif /* NUMERICEDIT_H_ */
