/* Copyright 2013-2015 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "threeDModelEngineApi.h"

#include <QtCore/qmath.h>

#include <qrkernel/settingsManager.h>
#include <qrkernel/logging.h>
#include <qrutils/mathUtils/math.h>
#include <qrutils/mathUtils/geometry.h>
/// @todo: Get rid of it!
#include <kitBase/robotModel/robotParts/touchSensor.h>
#include <kitBase/robotModel/robotParts/colorSensorFull.h>
#include <kitBase/robotModel/robotParts/colorSensorPassive.h>
#include <kitBase/robotModel/robotParts/colorSensorRed.h>
#include <kitBase/robotModel/robotParts/colorSensorGreen.h>
#include <kitBase/robotModel/robotParts/colorSensorBlue.h>

#include "twoDModel/engine/twoDModelGuiFacade.h"
#include "twoDModel/engine/model/model.h"
#include "twoDModel/engine/model/constants.h"
#include "twoDModel/engine/view/twoDModelWidget.h"

#include "view/scene/twoDModelScene.h"
#include "view/scene/robotItem.h"
#include "view/scene/fakeScene.h"

#include "src/engine/items/wallItem.h"
#include "src/engine/items/colorFieldItem.h"
#include "src/engine/items/ellipseItem.h"
#include "src/engine/items/stylusItem.h"
#include "src/engine/items/regions/ellipseRegion.h"
#include "src/engine/items/regions/rectangularRegion.h"

//
//#include <iostream>

//using namespace std;
//

using namespace twoDModel;
using namespace kitBase::robotModel;
using namespace twoDModel::model;

ThreeDModelEngineApi::ThreeDModelEngineApi(model::Model &model, view::TwoDModelWidget &view)
	: mModel(model)
	, mView(view)
	, mFakeScene(new view::FakeScene(mModel.worldModel()))
	, mGuiFacade(new engine::TwoDModelGuiFacade(mView))
{
}

ThreeDModelEngineApi::~ThreeDModelEngineApi()
{
}

// Block for 3D model

//void ThreeDModelEngineApi::initParameters3DModel(int clientID,
//												 int frontLeftHandle, int frontRightHandle,
//												 int backLeftHandle, int backRightHandle,
//												 int sonarSensorHandle)
void ThreeDModelEngineApi::initParameters3DModel(int const * parameters)
{
//	this->clientID = clientID;

//	this->frontLeftHandle = frontLeftHandle;
//	this->frontRightHandle = frontRightHandle;
//	this->backLeftHandle = backLeftHandle;
//	this->backRightHandle = backRightHandle;

//	this->sonarSensorHandle = sonarSensorHandle;

	this->clientID = parameters[0];

	this->frontLeftHandle = parameters[1];
	this->frontRightHandle = parameters[2];
	this->backLeftHandle = parameters[3];
	this->backRightHandle = parameters[4];

	this->sonarSensorHandle = parameters[5];

	this->rightHandle = parameters[6];
	this->leftHandle = parameters[7];
}

//

void ThreeDModelEngineApi::setNewMotor(int speed, uint degrees, const PortInfo &port, bool breakMode)
{
	// Block for 3D model

	// Connect must be only one on every port! (in the future)

//	cout << "Port = " << (string)port.name() << endl;


	if (port.name() == QString("M3")) {
		simxSetJointTargetVelocity(clientID, backLeftHandle, (float)speed * 10.0f, simx_opmode_oneshot);

		simxSetJointTargetVelocity(clientID, leftHandle, (float)speed * 0.0f, simx_opmode_oneshot);
	}

	if (port.name() == QString("M4")) {
		simxSetJointTargetVelocity(clientID, backRightHandle, (float)speed * 10.0f, simx_opmode_oneshot);

		simxSetJointTargetVelocity(clientID, rightHandle, (float)speed * 0.0f, simx_opmode_oneshot);
	}


	//simxSetJointTargetVelocity(clientID, frontLeftHandle, (float)speed * 0.1f, simx_opmode_oneshot);
	//simxSetJointTargetVelocity(clientID, frontRightHandle, -(float)speed * 0.1f, simx_opmode_oneshot);
//	simxSetJointTargetVelocity(clientID, backLeftHandle, (float)speed * 1000.0f, simx_opmode_oneshot);
//	simxSetJointTargetVelocity(clientID, backRightHandle, -(float)speed * 1000.0f, simx_opmode_oneshot);

	//

//	mModel.robotModels()[0]->setNewMotor(speed, degrees, port, breakMode);
}

int ThreeDModelEngineApi::readEncoder(const PortInfo &port) const
{
	return mModel.robotModels()[0]->readEncoder(port);
}

void ThreeDModelEngineApi::resetEncoder(const PortInfo &port)
{
	mModel.robotModels()[0]->resetEncoder(port);
}

int ThreeDModelEngineApi::readTouchSensor(const PortInfo &port) const
{
	if (!mModel.robotModels()[0]->configuration().type(port).isA<robotParts::TouchSensor>()) {
		return touchSensorNotPressedSignal;
	}

	QPair<QPointF, qreal> const neededPosDir = countPositionAndDirection(port);
	const QPointF position(neededPosDir.first);
	const qreal rotation = neededPosDir.second / 180 * mathUtils::pi;
	const QSizeF size = mModel.robotModels()[0]->sensorRect(port, position).size();

	QPainterPath sensorPath;
	const qreal touchRegionRadius = size.height() / 2;
	const qreal stickCenter = size.width() / 2 - touchRegionRadius;
	// (0,0) in sensor coordinates is sensor`s center
	const QPointF ellipseCenter = QPointF(stickCenter * cos(rotation), stickCenter * sin(rotation));
	sensorPath.addEllipse(position + ellipseCenter, touchRegionRadius, touchRegionRadius);

	const bool pressed = mModel.worldModel().checkCollision(sensorPath);
	return pressed ? touchSensorPressedSignal : touchSensorNotPressedSignal;
}

int ThreeDModelEngineApi::readSonarSensor(const PortInfo &port) const
{
	// Block for 3D model

	simxUChar sensorTrigger = 0;
	simxFloat * sensorPoint = new simxFloat[3];

	if (simxReadProximitySensor(clientID, sonarSensorHandle, &sensorTrigger, sensorPoint, NULL, NULL, simx_opmode_streaming) == simx_return_ok) {
		if (sensorTrigger) {
			simxFloat point1 = sensorPoint[0] * 100.0;
			simxFloat point2 = sensorPoint[1] * 100.0;
			simxFloat point3 = sensorPoint[2] * 100.0;

			float dist = sqrt(point1 * point1 + point2 * point2 + point3 * point3);
			delete[] sensorPoint;

			return (int)dist;
		}
	}

	delete[] sensorPoint;

	return 0;

	//

//	QPair<QPointF, qreal> neededPosDir = countPositionAndDirection(port);
//	const int res = mModel.worldModel().sonarReading(neededPosDir.first, neededPosDir.second);

//	return mModel.settings().realisticSensors() ? spoilSonarReading(res) : res;
}

QVector<int> ThreeDModelEngineApi::readAccelerometerSensor() const
{
	return mModel.robotModels()[0]->accelerometerReading();
}

QVector<int> ThreeDModelEngineApi::readGyroscopeSensor() const
{
	return mModel.robotModels()[0]->gyroscopeReading();
}

int ThreeDModelEngineApi::spoilSonarReading(const int distance) const
{
	const qreal ran = mathUtils::Math::gaussianNoise(spoilSonarDispersion);
	return mathUtils::Math::truncateToInterval(0, 255, round(distance + ran));
}

int ThreeDModelEngineApi::readColorSensor(const PortInfo &port) const
{
	const QImage image = areaUnderSensor(port, 1.0);
	QHash<uint, int> countsColor;

	const uint *data = reinterpret_cast<const uint *>(image.bits());
	const int n = image.byteCount() / 4;
	for (int i = 0; i < n; ++i) {
		const uint color = mModel.settings().realisticSensors() ? spoilColor(data[i]) : data[i];
		++countsColor[color];
	}

	if (mModel.robotModels()[0]->configuration().type(port).isA<robotParts::ColorSensorFull>()) {
		return readColorFullSensor(countsColor);
	} else if (mModel.robotModels()[0]->configuration().type(port).isA<robotParts::ColorSensorPassive>()) {
		return readColorNoneSensor(countsColor, n);
	} else if (mModel.robotModels()[0]->configuration().type(port).isA<robotParts::ColorSensorRed>()) {
		return readSingleColorSensor(red, countsColor, n);
	} else if (mModel.robotModels()[0]->configuration().type(port).isA<robotParts::ColorSensorGreen>()) {
		return readSingleColorSensor(green, countsColor, n);
	} else if (mModel.robotModels()[0]->configuration().type(port).isA<robotParts::ColorSensorBlue>()) {
		return readSingleColorSensor(blue, countsColor, n);
	}

	QLOG_ERROR() << "Incorrect 2d model sensor configuration";
	return 0;
}

uint ThreeDModelEngineApi::spoilColor(const uint color) const
{
	const qreal noise = mathUtils::Math::gaussianNoise(spoilColorDispersion);

	int r = round(((color >> 16) & 0xFF) + noise);
	int g = round(((color >> 8) & 0xFF) + noise);
	int b = round(((color >> 0) & 0xFF) + noise);
	const int a = (color >> 24) & 0xFF;

	r = mathUtils::Math::truncateToInterval(0, 255, r);
	g = mathUtils::Math::truncateToInterval(0, 255, g);
	b = mathUtils::Math::truncateToInterval(0, 255, b);

	return ((r & 0xFF) << 16) + ((g & 0xFF) << 8) + (b & 0xFF) + ((a & 0xFF) << 24);
}

QImage ThreeDModelEngineApi::areaUnderSensor(const PortInfo &port, qreal widthFactor) const
{
	DeviceInfo device = mModel.robotModels()[0]->configuration().type(port);
	if (device.isNull()) {
		device = mModel.robotModels()[0]->info().specialDevices()[port];
		if (device.isNull()) {
			return QImage();
		}
	}

	const QPair<QPointF, qreal> neededPosDir = countPositionAndDirection(port);
	const QPointF position = neededPosDir.first;
	const qreal direction = neededPosDir.second;
	const QRect imageRect = mModel.robotModels()[0]->info().sensorImageRect(device);
	const qreal width = imageRect.width() * widthFactor / 2.0;

	const QRectF sensorRectangle = QTransform().rotate(direction).map(QPolygonF(QRectF(imageRect))).boundingRect();
	const qreal rotationFactor = sensorRectangle.width() / imageRect.width();

	const qreal realWidth = width * rotationFactor;
	const QRectF scanningRect = QRectF(position.x() - realWidth, position.y() - realWidth
			, 2 * realWidth, 2 * realWidth);
	const QImage image(mFakeScene->render(scanningRect));
	const QPoint offset = QPointF(width, width).toPoint();
	const QImage rotated(image.transformed(QTransform().rotate(-(90 + direction))));
	const QRect realImage(rotated.rect().center() - offset + QPoint(1, 1), rotated.rect().center() + offset);
	QImage result(realImage.size(), QImage::Format_RGB32);
	result.fill(Qt::white);
	QPainter painter(&result);
	painter.drawImage(QRect(QPoint(), result.size()), rotated, realImage);
	painter.end();
	return result;
}

int ThreeDModelEngineApi::readColorFullSensor(QHash<uint, int> const &countsColor) const
{
	if (countsColor.isEmpty()) {
		return 0;
	}

	QList<int> const values = countsColor.values();
	int maxValue = INT_MIN;
	for (int value : values) {
		if (value > maxValue) {
			maxValue = value;
		}
	}

	const uint maxColor = countsColor.key(maxValue);
	switch (maxColor) {
	case (black):
		return 1;
	case (red):
		return 5;
	case (green):
		return 3;
	case (blue) :
		return 2;
	case (yellow):
		return 4;
	case (white):
		return 6;
	case (cyan):
		return 7;
	case (magenta):
		return 8;
	default:
		return 0;
	}
}

int ThreeDModelEngineApi::readSingleColorSensor(uint color, QHash<uint, int> const &countsColor, int n) const
{
	return (static_cast<double>(countsColor[color]) / static_cast<double>(n)) * 100.0;
}

int ThreeDModelEngineApi::readColorNoneSensor(QHash<uint, int> const &countsColor, int n) const
{
	double allWhite = static_cast<double>(countsColor[white]);

	QHashIterator<uint, int> i(countsColor);
	while(i.hasNext()) {
		i.next();
		const uint color = i.key();
		if (color != white) {
			const int b = (color >> 0) & 0xFF;
			const int g = (color >> 8) & 0xFF;
			const int r = (color >> 16) & 0xFF;
			const qreal k = qSqrt(static_cast<qreal>(b * b + g * g + r * r)) / 500.0;
			allWhite += static_cast<qreal>(i.value()) * k;
		}
	}

	return (allWhite / static_cast<qreal>(n)) * 100.0;
}

int ThreeDModelEngineApi::readLightSensor(const PortInfo &port) const
{
	// Must return 1023 on white and 0 on black normalized to percents
	// http://stackoverflow.com/questions/596216/formula-to-determine-brightness-of-rgb-color

	const QImage image = areaUnderSensor(port, 1.0);
	if (image.isNull()) {
		return 0;
	}

	uint sum = 0;
	const uint *data = reinterpret_cast<const uint *>(image.bits());
	const int n = image.byteCount() / 4;

	for (int i = 0; i < n; ++i) {
		const int color = mModel.settings().realisticSensors() ? spoilLight(data[i]) : data[i];
		const int b = (color >> 0) & 0xFF;
		const int g = (color >> 8) & 0xFF;
		const int r = (color >> 16) & 0xFF;
		// brightness in [0..256]
		const uint brightness = static_cast<uint>(0.2126 * r + 0.7152 * g + 0.0722 * b);

		sum += 4 * brightness; // 4 = max sensor value / max brightness value
	}

	const qreal rawValue = sum * 1.0 / n; // Average by whole region
	return rawValue * 100 / maxLightSensorValue; // Normalizing to percents
}

void ThreeDModelEngineApi::playSound(int timeInMs)
{
	mModel.robotModels()[0]->playSound(timeInMs);
}

bool ThreeDModelEngineApi::isMarkerDown() const
{
	return mModel.robotModels()[0]->markerColor() != Qt::transparent;
}

void ThreeDModelEngineApi::markerDown(const QColor &color)
{
	mModel.robotModels()[0]->markerDown(color);
}

void ThreeDModelEngineApi::markerUp()
{
	mModel.robotModels()[0]->markerUp();
}

utils::TimelineInterface &ThreeDModelEngineApi::modelTimeline()
{
	return mModel.timeline();
}

engine::TwoDModelDisplayInterface *ThreeDModelEngineApi::display()
{
	return mView.display();
}

engine::TwoDModelGuiFacade &ThreeDModelEngineApi::guiFacade() const
{
	return *mGuiFacade;
}

uint ThreeDModelEngineApi::spoilLight(const uint color) const
{
	const qreal noise = mathUtils::Math::gaussianNoise(spoilLightDispersion);

	if (noise > (1.0 - percentSaltPepperNoise / 100.0)) {
		return white;
	} else if (noise < (-1.0 + percentSaltPepperNoise / 100.0)) {
		return black;
	}

	return color;
}

QPair<QPointF, qreal> ThreeDModelEngineApi::countPositionAndDirection(const PortInfo &port) const
{
	RobotModel * const robotModel = mModel.robotModels()[0];
	const QVector2D sensorVector = QVector2D(robotModel->configuration().position(port) - rotatePoint);
	const QPointF rotatedVector = mathUtils::Geometry::rotateVector(sensorVector, robotModel->rotation()).toPointF();
	const QPointF position = robotModel->position() + rotatePoint + rotatedVector;
	const qreal direction = robotModel->configuration().direction(port) + robotModel->rotation();
	return { position, direction };
}

void ThreeDModelEngineApi::enableBackgroundSceneDebugging()
{
	// A crappy piece of code that must be never called in master branch,
	// but this is a pretty convenient way to debug a fake scene.
	// If called from constructor (where robotModels are not initialized yet)
	// then NXT and TRIK 2D fake scenes will be shown.
	QGraphicsView * const fakeScene = new QGraphicsView;
	fakeScene->setScene(mFakeScene.data());
	QTimer * const timer = new QTimer;
	QObject::connect(timer, SIGNAL(timeout()), mFakeScene.data(), SLOT(update()));
	timer->setInterval(300);
	timer->setSingleShot(false);
	fakeScene->setMinimumWidth(700);
	fakeScene->setMinimumHeight(600);
	fakeScene->setWindowFlags(fakeScene->windowFlags() | Qt::WindowStaysOnTopHint);
	fakeScene->setVisible(mModel.robotModels().isEmpty()
			? true
			: mModel.robotModels()[0]->info().robotId().contains("trik"));
	timer->start();
}
