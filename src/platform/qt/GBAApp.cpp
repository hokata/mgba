/* Copyright (c) 2013-2014 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "GBAApp.h"

#include "AudioProcessor.h"
#include "GameController.h"

#include <QFileOpenEvent>

extern "C" {
#include "platform/commandline.h"
#include "util/socket.h"
}

using namespace QGBA;

GBAApp::GBAApp(int& argc, char* argv[])
	: QApplication(argc, argv)
	, m_window(&m_configController)
{
#ifdef BUILD_SDL
	SDL_Init(SDL_INIT_NOPARACHUTE);
#endif

	SocketSubsystemInit();

	QApplication::setApplicationName(PROJECT_NAME);
	QApplication::setApplicationVersion(PROJECT_VERSION);

#ifndef Q_OS_MAC
	m_window.show();
#endif

	GBAArguments args;
	if (m_configController.parseArguments(&args, argc, argv)) {
		m_window.argumentsPassed(&args);
	} else {
		m_window.loadConfig();
	}
	freeArguments(&args);

	AudioProcessor::setDriver(static_cast<AudioProcessor::Driver>(m_configController.getQtOption("audioDriver").toInt()));
	m_window.controller()->reloadAudioDriver();

#ifdef Q_OS_MAC
	m_window.show();
#endif
}

bool GBAApp::event(QEvent* event) {
	if (event->type() == QEvent::FileOpen) {
		m_window.controller()->loadGame(static_cast<QFileOpenEvent*>(event)->file());
		return true;
	}
	return QApplication::event(event);
}
