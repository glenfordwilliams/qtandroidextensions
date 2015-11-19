/*
	Offscreen Android Views library for Qt

	Authors:
	Vyacheslav O. Koscheev <vok1980@gmail.com>
	Ivan Avdeev marflon@gmail.com
	Sergey A. Galin sergey.galin@gmail.com

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

#include "QAndroidCellDataProvider.h"
#include <QAndroidQPAPluginGap.h>


namespace Mobility {

Q_DECL_EXPORT void JNICALL Java_CellListener_cellUpdate(JNIEnv *, jobject, jlong native_ptr, jint cid, jint lac, jint mcc, jint mnc, jint rssi)
{
	if (native_ptr)
	{
		void * vp = reinterpret_cast<void*>(native_ptr);
		Mobility::QAndroidCellDataProvider * proxy = reinterpret_cast<Mobility::QAndroidCellDataProvider*>(vp);
		
		if (proxy)
		{
			proxy->cellUpdate(cid, lac, mcc, mnc, rssi);
			return;
		}
	}

	qWarning() << __FUNCTION__ << "Zero param!";
}


static const JNINativeMethod methods[] = {
	{"getContext", "()Landroid/content/Context;", (void*)QAndroidQPAPluginGap::getCurrentContext},
	// private native void cellUpdate(long native_ptr, int cid, int lac, int mcc, int mnc, int rssi);
	{"cellUpdate", "(JIIIII)V", (void*)Java_CellListener_cellUpdate},
};




QAndroidCellDataProvider::QAndroidCellDataProvider(QObject * parent /*= 0*/) : 
	QObject(parent)
	, JniObjectLinker(reinterpret_cast<void*>(this), "ru.dublgis.androidhelpers.mobility.CellListener", methods, sizeof(methods))
{
}


QAndroidCellDataProvider::~QAndroidCellDataProvider()
{
}


void QAndroidCellDataProvider::start()
{
	handler()->callBool("start");
}


void QAndroidCellDataProvider::stop()
{
	handler()->callVoid("stop");
}


void QAndroidCellDataProvider::cellUpdate(int cid, int lac, int mcc, int mnc, int rssi)
{
	CellDataPtr new_data(new CellData);
	new_data->cellId = cid;
	new_data->locationAreaCode = lac;
	new_data->mobileCountryCode = mcc;
	new_data->mobileNetworkCode = mnc;
	new_data->signalStrength = rssi;
	new_data->timingAdvance = 0;

	if (last_data_)
	{
		if (*new_data==*last_data_)
		{
			return;
		}
	}
	
	last_data_ = new_data;
	emit update(last_data_);
}


CellDataPtr QAndroidCellDataProvider::getLastData()
{
	return last_data_;
}

}
