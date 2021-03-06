/* Copyright 2014-2017 QReal Research Group
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

#include "trikKitInterpreterCommon/trikAdditionalPreferences.h"
#include "ui_trikAdditionalPreferences.h"

#include <qrkernel/settingsManager.h>

using namespace trik;
using namespace qReal;

TrikAdditionalPreferences::TrikAdditionalPreferences(const QStringList &realRobotNames, QWidget *parent)
	: AdditionalPreferences(parent)
	, mUi(new Ui::TrikAdditionalPreferences)
	, mRealRobotNames(realRobotNames)
{
	mUi->setupUi(this);
	mUi->robotImagePicker->configure("trikRobot2DImage", tr("2D robot image:"));
}

TrikAdditionalPreferences::~TrikAdditionalPreferences()
{
	delete mUi;
}

void TrikAdditionalPreferences::save()
{
	SettingsManager::setValue("TrikTcpServer", mUi->tcpServerLineEdit->text());
	mUi->robotImagePicker->save();
	emit settingsChanged();
}

void TrikAdditionalPreferences::restoreSettings()
{
	mUi->tcpServerLineEdit->setText(SettingsManager::value("TrikTcpServer").toString());
	mUi->robotImagePicker->restore();
}

void TrikAdditionalPreferences::onRobotModelChanged(kitBase::robotModel::RobotModelInterface * const robotModel)
{
	const bool isReal = !robotModel->name().contains("TwoD");
	mUi->tcpSettingsGroupBox->setVisible(isReal);
}
