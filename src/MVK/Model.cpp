/*
 * Model.cpp
 *
 *  Created on: 09-08-2013
 *      Author: przemo
 */

#include "Model.h"

ParametryObiektow Obiekt::parametryObiektow;
ParametrySmoka Smok::parametry;

Model::Model(int szerOkna,int wysOkna, bool ekran): wymiaryEkranu(Punkt(szerOkna,wysOkna)),pelnyEkran(ekran)
{
	wyswietlenieInstrukcji=true;
	wypelnienieCelownika=false;

	FabrykaPrzedmiotow::zwrocInstancje()->ustawKontenery(&mury,&zaslony);
	FabrykaPociskow::zwrocInstancje()->ustawKontenery(&plomienie,&strzaly, &belty, &pociskiBalistyczne);
	FabrykaLudzi::zwrocInstancje()->ustawKontenery(&strzelcy,&balisty);

	reset();
}

void Model::reset()
{
	wyswietlenieOdNowa=false;

	strzelcy.wyczysc();
	balisty.wyczysc();

	plomienie.wyczysc();
	strzaly.wyczysc();
	belty.wyczysc();
	pociskiBalistyczne.wyczysc();
	mury.wyczysc();
	zaslony.wyczysc();

	smok.reset();

	//FabrykaPrzedmiotow::zwrocInstancje()->stworzPrzedmiot(FabrykaPrzedmiotow::zaslona,Punkt(1200,600));
	//FabrykaPrzedmiotow::zwrocInstancje()->stworzPrzedmiot(FabrykaPrzedmiotow::zaslona,Punkt(1500,600));

	//FabrykaPrzedmiotow::zwrocInstancje()->stworzPrzedmiot(FabrykaPrzedmiotow::sredniMur,Punkt(1400,300));

	//FabrykaLudzi::zwrocInstancje()->stworzCzlowieka(FabrykaLudzi::lucznik,Punkt(1300,130));
	//FabrykaLudzi::zwrocInstancje()->stworzCzlowieka(FabrykaLudzi::krzyzowiec,Punkt(1400,130));
	//FabrykaLudzi::zwrocInstancje()->stworzCzlowieka(FabrykaLudzi::balista,Punkt(1400,200),false);
	FabrykaLudzi::zwrocInstancje()->stworzCzlowieka(FabrykaLudzi::balista,Punkt(1000,200),true);

	/*for(int i=0;i<10;i++)
	{
		int x = rand()%3000+5000;
		FabrykaLudzi::zwrocInstancje()->stworzCzlowieka(FabrykaLudzi::lucznik,Punkt(x,130));
	}//*/
	smok.ustawPozycje(Punkt(1000,100));
	kamera.ustawPozycje(smok.zwrocPozycje());
	myszka.ustawX(wymiaryEkranu.x/2);
	myszka.ustawY(wymiaryEkranu.y/2);
	wyznaczKolejnyStanObiektow();
	wyznaczKolejnyStanObiektow();
}

//####################################################KOLEJNY STAN#######################################################

void Model::wyznaczKolejnyStan()
{
	if(wyswietlenieInstrukcji)
	{
		if(klawiatura.czyWcisnietoSpacje()) wyswietlenieInstrukcji=false;
	}
	else
	{
		if(wyswietlenieOdNowa && klawiatura.czyWcisnietoSpacje()) reset();
		wyznaczKolejnyStanObiektow();
	}
}

void Model::wyznaczKolejnyStanObiektow()
{
	//Liczymy wspolrzedne myszki wzgledem glowy Smoka i Wyznaczamy kolejny stan Smoka
	Punkt punktMyszkiSmoka;
	if(kamera.zwrocY()<wymiaryEkranu.y/2) {punktMyszkiSmoka.y=wymiaryEkranu.y-smok.zwrocPozycje().y+smok.zwrocPozycjeGlowy().y-myszka.zwrocY();}
						else {punktMyszkiSmoka.y=wymiaryEkranu.y/2-smok.zwrocPozycje().y+kamera.zwrocY()+smok.zwrocPozycjeGlowy().y-myszka.zwrocY();}
	punktMyszkiSmoka.x=wymiaryEkranu.x/2+smok.zwrocPozycje().x-kamera.zwrocX()+smok.zwrocPozycjeGlowy().x-myszka.zwrocX();
	Myszka myszkaSmoka(punktMyszkiSmoka,myszka.zwrocLPM(),myszka.zwrocPPM());
	smok.wyznaczKolejnyStan(&klawiatura,&myszkaSmoka);
	smok.wyznaczKlatkeAnimacji();
	smok.wyznaczPrzestrzenKolizji();

	//Wyznaczamy kolejny stan kamery
	Punkt punktMyszkiKamery(myszka.zwrocX(),myszka.zwrocY());
	Obiekt* obiektDoSledzenia = (&smok);
	if(smok.czyZwroconyWPrawo() && punktMyszkiKamery.x<3*wymiaryEkranu.x/4) punktMyszkiKamery.x=3*wymiaryEkranu.x/4;
	if(!smok.czyZwroconyWPrawo() && punktMyszkiKamery.x>wymiaryEkranu.x/4) punktMyszkiKamery.x=wymiaryEkranu.x/4;
	punktMyszkiKamery.x+=obiektDoSledzenia->zwrocPozycje().x+-wymiaryEkranu.x/2;
	punktMyszkiKamery.y=obiektDoSledzenia->zwrocPozycje().y-myszka.zwrocY()+wymiaryEkranu.y/2;
	kamera.wyznaczKolejnyStan(punktMyszkiKamery);

	//Strzelcy
	strzelcy.ustawCel(&smok);
	strzelcy.wyznaczKolejnyStan();
	strzelcy.wyznaczKlatkeAnimacji();

	//Balisty
	balisty.ustawCel(&smok);
	balisty.wyznaczKolejnyStan();
	balisty.wyznaczKlatkeAnimacji();

	//Pociski
	plomienie.wyznaczKolejnyStan();
	plomienie.wyznaczKlatkeAnimacji();
	strzaly.wyznaczKolejnyStan();
	strzaly.wyznaczKlatkeAnimacji();
	belty.wyznaczKolejnyStan();
	belty.wyznaczKlatkeAnimacji();
	pociskiBalistyczne.wyznaczKolejnyStan();
    pociskiBalistyczne.wyznaczKlatkeAnimacji();
    pociskiBalistyczne.wyznaczPrzestrzenKolizji();

	zaslony.wyznaczKolejnyStan();
	zaslony.wyznaczKlatkeAnimacji();

	if((balisty.czyPusty() && strzelcy.czyPusty()) || smok.czyZniszczony()) wyswietlenieOdNowa=true;

	//									Sadzenie krzyzowcow
	/*
	std::list<Punkt> pozycja = plomienie.zwrocPozycje();
	for(std::list<Punkt>::iterator i=pozycja.begin(); i!=pozycja.end();i++)
	{
		if(i->y<90) fabrykaLudzi.stworzCzlowieka(FabrykaLudzi::krzyzowiec,Punkt(i->x,130));
	}
	//*/
	//std::cout << strzelcy.zwrocObiekty().size() << "\n";
	//								Losowe tworzenie krzyzowcow

	/*if(strzelcy.zwrocObiekty().size()<100)
	{
		int x = rand()%10000+1000;
		FabrykaLudzi::zwrocInstancje()->stworzCzlowieka(FabrykaLudzi::krzyzowiec,Punkt(x,130));
	}
	//*/
	obsluzKolizje();
}

//##########################################################KOLIZJE#########################################################

void Model::obsluzKolizje()
{
	//Strzelcy kontra plomienie
	std::list<Postac*> listaStrzelcow = strzelcy.zwrocObiekty();
	for(std::list<Postac*>::iterator i=listaStrzelcow.begin();i!=listaStrzelcow.end();i++)
	{
		plomienie.sprawdzKolizje((Obiekt*)(*i),zniszczPocisk,zadajObrazenia,PrzestrzenKolizji::okrag,true);
	}
	//Balisty kontra plomienie
	std::list<Postac*> listaBalist = balisty.zwrocObiekty();
	for(std::list<Postac*>::iterator i=listaBalist.begin();i!=listaBalist.end();i++)
	{
		plomienie.sprawdzKolizje((Obiekt*)(*i),zniszczPocisk,zadajObrazenia,PrzestrzenKolizji::prostokat,true);
	}

	//Smok kontra strzaly
	strzaly.sprawdzKolizje(&smok,usun,zadajObrazenia,PrzestrzenKolizji::okrag);
	belty.sprawdzKolizje(&smok,usun,zadajObrazenia,PrzestrzenKolizji::okrag);
	pociskiBalistyczne.sprawdzKolizje(&smok,nic,zadajObrazenia,PrzestrzenKolizji::okrag);

	//Mury
	std::list<Mur> *listaMurow = mury.zwrocObiekty();
	for(std::list<Mur>::iterator i=listaMurow->begin();i!=listaMurow->end();i++)
	{
		plomienie.sprawdzKolizje((Obiekt*)&(*i),kolizjaPlomieniazMurem,nic,PrzestrzenKolizji::prostokat,true);
		belty.sprawdzKolizje((Obiekt*)&(*i),kolizjaPlomieniazMurem,nic,PrzestrzenKolizji::prostokat,true);
		strzaly.sprawdzKolizje((Obiekt*)&(*i),kolizjaPlomieniazMurem,nic,PrzestrzenKolizji::prostokat,true);
		pociskiBalistyczne.sprawdzKolizje((Obiekt*)&(*i),kolizjaPlomieniazMurem,nic,PrzestrzenKolizji::prostokat,true);
	}
	//Plomienie Zaslony
	std::list<Zaslona> *listaZaslon = zaslony.zwrocObiekty();
	for(std::list<Zaslona>::iterator i=listaZaslon->begin();i!=listaZaslon->end();i++)
	{
		plomienie.sprawdzKolizje((Obiekt*)&(*i),zniszczPocisk,zadajObrazenia,PrzestrzenKolizji::prostokat,true);
	}

	mury.sprawdzKolizje(&smok,nic,kolizjaSmokaZMurem,PrzestrzenKolizji::prostokat,false);
	zaslony.sprawdzKolizje(&smok,nic,kolizjaSmokaZPlatforma,PrzestrzenKolizji::prostokat,false);
}

void Model::zniszcz(Obiekt *o,Obiekt *o2,Punkt punktKolizji)
{
	o->zniszcz();
}
void Model::zniszczPocisk(Obiekt *o,Obiekt *o2,Punkt punktKolizji)
{
	if(!o->czyZniszczony())
	{
		if(punktKolizji.x>o2->zwrocPozycje().x) punktKolizji.x+=2;
		else punktKolizji.x-=2;
		if(punktKolizji.y>o2->zwrocPozycje().y) punktKolizji.y+=2;
		else punktKolizji.y-=2;

		o->ustawPozycje(punktKolizji);
		o->ustawPunktZaczepu(o2);
	}
	o->zniszcz();
}

void Model::usun(Obiekt *o,Obiekt *o2,Punkt punktKolizji)
{
	o->zniszcz();
	o->usun();
}

void Model::nic(Obiekt *o,Obiekt *o2,Punkt punktKolizji)
{
	return;
}

void Model::zadajObrazenia(Obiekt*o, Obiekt *o2, Punkt punktKolizji)
{
	o->zadajObrazenia(o2->zwrocObrazenia());
}

void Model::kolizjaSmokaZMurem(Obiekt*o, Obiekt *o2, Punkt punktKolizji)
{
	assert("Smok nie ma ustalonego prostokata kolizji" && !o->zwrocPrzestrzenKolizji()->zwrocProstokaty()->empty());
	ProstokatKolizji prostokat1 = (*(o->zwrocPrzestrzenKolizji()->zwrocProstokaty()))[0];
	ProstokatKolizji prostokat2 = (*(o2->zwrocPrzestrzenKolizji()->zwrocProstokaty()))[0];
	double minY = o2->zwrocPozycje().y+prostokat1.zwrocBok2()/2+prostokat2.zwrocBok2()/2;
	double maxY = o2->zwrocPozycje().y-prostokat1.zwrocBok2()/2-prostokat2.zwrocBok2()/2;
	double minX = o2->zwrocPozycje().x-prostokat1.zwrocBok1()/2-prostokat2.zwrocBok1()/2;
	double maxX = o2->zwrocPozycje().x+prostokat1.zwrocBok1()/2+prostokat2.zwrocBok1()/2;


	if(o->zwrocPozycje().y+prostokat1.zwrocBok2()/2>minY || o->zwrocPredkosc().y<-prostokat1.zwrocBok2()/2)
	{
		if(prostokat1.zwrocPozycje().x-6*prostokat1.zwrocBok1()/8<minX)
			o->ustawPozycje(Punkt(o->zwrocPozycje().x+(minX-prostokat1.zwrocPozycje().x)-2,o->zwrocPozycje().y+(minX-prostokat1.zwrocPozycje().x)/2));
		else if(prostokat1.zwrocPozycje().x+6*prostokat1.zwrocBok1()/8>maxX)
			o->ustawPozycje(Punkt(o->zwrocPozycje().x+(maxX-prostokat1.zwrocPozycje().x)+2,o->zwrocPozycje().y-(maxX-prostokat1.zwrocPozycje().x)/2));
		else
			o->postawNaZiemi(minY);
	}
	else if(o->zwrocPozycje().x-prostokat1.zwrocBok1()/2<minX || o->zwrocPredkosc().x>prostokat1.zwrocBok1()/2)
	{
		o->zatrzymajNaScianie();
		if(o->zwrocPozycje().y-prostokat1.zwrocBok2()/32>=maxY)
			o->ustawPozycje(Punkt(o->zwrocPozycje().x+(minX-prostokat1.zwrocPozycje().x)-2,o->zwrocPozycje().y));
	}
	else if(o->zwrocPozycje().x+prostokat1.zwrocBok1()/2>maxX || o->zwrocPredkosc().x<-prostokat1.zwrocBok1()/2)
	{
		o->zatrzymajNaScianie();
		if(o->zwrocPozycje().y-prostokat1.zwrocBok2()/32>=maxY)
			o->ustawPozycje(Punkt(o->zwrocPozycje().x+(maxX-prostokat1.zwrocPozycje().x)+2,o->zwrocPozycje().y));
	}

	else if(o->zwrocPozycje().y-prostokat1.zwrocBok2()/2<maxY || o->zwrocPredkosc().y>prostokat1.zwrocBok2()/2)
	{
		o->zatrzymajNaSuficie();
	}

	if(o->zwrocPozycje().y-prostokat1.zwrocBok2()/32<maxY)
	{
		o->zatrzymajNaSuficie();
	}


}

void Model::kolizjaSmokaZPlatforma(Obiekt*o, Obiekt *o2, Punkt punktKolizji)
{
	assert("Smok nie ma ustalonego prostokata kolizji" && !o->zwrocPrzestrzenKolizji()->zwrocProstokaty()->empty());
	ProstokatKolizji prostokat1 = (*(o->zwrocPrzestrzenKolizji()->zwrocProstokaty()))[0];
	ProstokatKolizji prostokat2 = (*(o2->zwrocPrzestrzenKolizji()->zwrocProstokaty()))[0];
	double minY = o2->zwrocPozycje().y+prostokat1.zwrocBok2()/2+prostokat2.zwrocBok2()/2;
	double maxY = o2->zwrocPozycje().y-prostokat1.zwrocBok2()/2-prostokat2.zwrocBok2()/2;
	double minX = o2->zwrocPozycje().x-prostokat1.zwrocBok1()/2-prostokat2.zwrocBok1()/2;
	double maxX = o2->zwrocPozycje().x+prostokat1.zwrocBok1()/2+prostokat2.zwrocBok1()/2;


	if((o->zwrocPozycje().y+prostokat1.zwrocBok2()/8>minY || o->zwrocPredkosc().y<-prostokat1.zwrocBok2()/8) && prostokat1.zwrocPozycje().x-6*prostokat1.zwrocBok1()/8>minX && prostokat1.zwrocPozycje().x+6*prostokat1.zwrocBok1()/8<maxX )
	{
			o->postawNaZiemi(minY+prostokat2.zwrocPozycjeWzgledemObiektu().y);
	}


}

void Model::kolizjaPlomieniazMurem(Obiekt *o,Obiekt *o2,Punkt punktKolizji)
{
	ProstokatKolizji prostokat=(*(o2->zwrocPrzestrzenKolizji()->zwrocProstokaty()))[0];
	Punkt pozycja1=o->zwrocPozycje();
	Punkt pozycja2=(o->zwrocPozycje())-(o->zwrocPredkosc());
	if(pozycja1.x>prostokat.zwrocPozycje().x-prostokat.zwrocBok1()/2 && pozycja1.x<prostokat.zwrocPozycje().x+prostokat.zwrocBok1()/2 &&
	   pozycja1.y>prostokat.zwrocPozycje().y-prostokat.zwrocBok2()/2 && pozycja1.y<prostokat.zwrocPozycje().y+prostokat.zwrocBok2()/2 &&
	   (
	     (pozycja2.x>prostokat.zwrocPozycje().x-prostokat.zwrocBok1()/2 && pozycja2.x<prostokat.zwrocPozycje().x+prostokat.zwrocBok1()/2) &&
	     (pozycja2.y>prostokat.zwrocPozycje().y-prostokat.zwrocBok2()/2 && pozycja2.y<prostokat.zwrocPozycje().y+prostokat.zwrocBok2()/2)
	   )
	  ) //Jeśli plomien pojawil sie wewnatrz muru to ma byc zniszczony natychmiastowo
	{
		o->usun();
	}
	else if(!o->czyZniszczony())
	{
		o->ustawPozycje(punktKolizji);
		o->ustawPunktZaczepu(o2);
	}
	o->zniszcz();
}
