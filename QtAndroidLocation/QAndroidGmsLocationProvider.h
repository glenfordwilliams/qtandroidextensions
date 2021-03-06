/*
  Offscreen Android Views library for Qt

  Author:
  Vyacheslav O. Koscheev <vok1980@gmail.com>

  Distrbuted under The BSD License

  Copyright (c) 2015, DoubleGIS, LLC.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the DoubleGIS, LLC nor the names of its contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QJniHelpers.h>
#include <QtPositioning/QGeoPositionInfo>


/*!
 * A class for geting location from Google Play Services
 */
class QAndroidGmsLocationProvider: public QObject
{
	Q_OBJECT

public:
	enum enStatus
	{
		S_DISCONNECTED 		= 0,
		S_CONNECTED 		= 1,
		S_CONNECT_ERROR 	= 2,
		S_CONNECT_SUSPEND	= 3,
		S_REQUEST_SUCCESS	= 4,
		S_REQUEST_FAIL		= 5,
	};

	enum enPriority
	{
		PRIORITY_HIGH_ACCURACY            = 0x00000064, /// the most accurate locations available
		PRIORITY_BALANCED_POWER_ACCURACY  = 0x00000066, /// "block" level accuracy.
		PRIORITY_LOW_POWER                = 0x00000068, /// "city" level accuracy.
		PRIORITY_NO_POWER                 = 0x00000069, /// the best accuracy possible with zero additional power consumption.
	};

public:
	QAndroidGmsLocationProvider(QObject * parent = 0);
	virtual ~QAndroidGmsLocationProvider();

public slots:
	void startUpdates();
	void stopUpdates();
	virtual void requestUpdate(int timeout = 0);

private slots:
	void onRequestTimeout();

signals:
	void statusChanged(int);
	void locationRecieved(const QGeoPositionInfo&);

public:
	static bool isAvailable(jboolean allowDialog);
	static int getGmsVersion();
	void setUpdateInterval(int64_t reqiredInterval, int64_t minimumInterval);
	void setPriority(enPriority priority);
	QGeoPositionInfo lastKnownPosition() const;

	static void preloadJavaClasses();

private:
	void stopUpdates(jlong requestId);
	void onStatusChanged(int status);
	void onLocationRecieved(const QGeoPositionInfo &location, jboolean initial, jlong requestId);

private:
	Q_DISABLE_COPY(QAndroidGmsLocationProvider)
	friend void JNICALL Java_GooglePlayServiceLocationProvider_locationRecieved(JNIEnv * env, jobject, jlong param, jobject location, jboolean initial, jlong requestId);
	friend void JNICALL Java_GooglePlayServiceLocationProvider_locationStatus(JNIEnv * env, jobject, jlong param, jint state);

private:
	QScopedPointer<QJniObject> handler_;
	QGeoPositionInfo lastLocation_;
	mutable QMutex lastLocationSync_;

	jlong reqiredInterval_; 
	jlong minimumInterval_;
	enPriority priority_;

	jlong regularUpdadesId_;
	jlong requestUpdadesId_;

	QTimer requestTimer_;
};


