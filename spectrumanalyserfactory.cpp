/*************************************************************************

	Copyright 2011 Ibrahim Sha'ath

	This file is part of KeyFinder.

	KeyFinder is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	KeyFinder is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with KeyFinder.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#include "spectrumanalyserfactory.h"

SpectrumAnalyserWrapper::SpectrumAnalyserWrapper(char t, char p, int f, char w, SpectrumAnalyser* s){
	type = t;
	fftpp = p;
	frate = f;
	window = w;
	sa = s;
}

SpectrumAnalyserWrapper::~SpectrumAnalyserWrapper(){
	delete sa;
}

char SpectrumAnalyserWrapper::getType() const{
	return type;
}

char SpectrumAnalyserWrapper::getFftPostProcessor() const{
	return fftpp;
}

int SpectrumAnalyserWrapper::getFramerate() const{
	return frate;
}

char SpectrumAnalyserWrapper::getTemporalWindow() const{
	return window;
}

SpectrumAnalyser* SpectrumAnalyserWrapper::getSpectrumAnalyser() const{
	return sa;
}

SpectrumAnalyserFactory* SpectrumAnalyserFactory::inst = NULL;

SpectrumAnalyserFactory* SpectrumAnalyserFactory::getInstance(){
	if(inst == NULL)
		inst = new SpectrumAnalyserFactory();
	return inst;
}

SpectrumAnalyserFactory::SpectrumAnalyserFactory(){
	analysers = std::vector<SpectrumAnalyserWrapper*>(0);
}

SpectrumAnalyserFactory::~SpectrumAnalyserFactory(){
	for(int i=0; i<(signed)analysers.size(); i++)
		delete analysers[i];
	inst = NULL;
}

SpectrumAnalyser* SpectrumAnalyserFactory::getSpectrumAnalyser(int frameRate, const Preferences& prefs){
	QMutexLocker locker(&mutex); // This function should be accessed by only one thread at a time
	char chkType = prefs.getSpectrumAnalyser();
	char chkFpp = prefs.getFftPostProcessor();
	char chkWin = prefs.getTemporalWindow();
	for(int i=0; i<(signed)analysers.size(); i++)
		if(
			 analysers[i]->getType() == chkType
			 && analysers[i]->getFftPostProcessor() == chkFpp
			 && analysers[i]->getFramerate() == frameRate
			 && analysers[i]->getTemporalWindow() == chkWin
		)
			return analysers[i]->getSpectrumAnalyser();
	if(chkType == 'g')
		analysers.push_back(new SpectrumAnalyserWrapper(chkType, chkFpp, frameRate, chkWin, new GoertzelAnalyser(frameRate,prefs)));
	else
		analysers.push_back(new SpectrumAnalyserWrapper(chkType, chkFpp, frameRate, chkWin, new FftwAnalyser(frameRate,prefs)));
	return analysers[analysers.size()-1]->getSpectrumAnalyser();
}
