#ifndef COLORCOMBOBOX_H_
#define COLORCOMBOBOX_H_

#include <QComboBox>
#include <QColor>

class ColorComboBox : public QComboBox
{
	Q_OBJECT

public:
	ColorComboBox(QWidget *parent = 0);

	void addColor(const QColor &color, const QString &name);
	void setColor(const QColor &color);
	QColor color() const;

	int colorCount() const;
	QSize sizeHint() const;

signals:
	void activated(const QColor &color);

private slots:
	void emitActivatedColor(int index);

private:
	QColor lastActivated;
};

#endif /* COLORCOMBOBOX_H_ */

