
#include "mdtAnalogIo.h"

 mdtAnalogIo::mdtAnalogIo(const QObject & parent) {
}

 mdtAnalogIo::~mdtAnalogIo() {
}

void mdtAnalogIo::setUnit(const QString & unit) {
}

QString mdtAnalogIo::unit() {
}

void mdtAnalogIo::setRange(double min, double max, int steps) {
}

double mdtAnalogIo::minimum() {
}

double mdtAnalogIo::maximum() {
}

void mdtAnalogIo::setValue(double value) {
}

double mdtAnalogIo::value() {
}

void mdtAnalogIo::setValueInt(int value) {
}

int mdtAnalogIo::valueInt() {
}

void mdtAnalogIo::valueChanged(double newValue,  ) {
}

void mdtAnalogIo::valueChanged(int address, double newValue) {
}

void mdtAnalogIo::unitChangedForUi(const QString & unit) {
}

void mdtAnalogIo::rangeChangedForUi(double min, double max) {
}

void mdtAnalogIo::valueChangedForUi(double newValue) {
}

void mdtAnalogIo::setValueFromUi(double value) {
}

