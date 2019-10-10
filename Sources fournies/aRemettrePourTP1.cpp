//
// Created by Mario Marchand on 16-12-29.
//

#include "DonneesGTFS.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include "coordonnees.h"


using namespace std;



ifstream leFichierStations;

//! \brief ajoute les lignes dans l'objet GTFS
//! \param[in] p_nomFichier: le nom du fichier contenant les lignes
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterLignes(const std::string &p_nomFichier)
{
//    map<char, CategorieBus> my_map = {
//            { "97BF0D", CategorieBus::METRO_BUS },
//            { "013888", CategorieBus::LEBUS},
//            { "E04503", CategorieBus::EXPRESS },
//            { "1A171B", CategorieBus::COUCHE_TARD },
//            { "003888", CategorieBus::COUCHE_TARD },
//    };

    ostringstream nomDufichier;

    nomDufichier << "../" << p_nomFichier;

    std::ifstream file(nomDufichier.str());
    std::string str;
    while(std::getline(file, str)){
        if(str == "route_id,agency_id,route_short_name,route_long_name,route_desc,route_type,route_url,route_color,route_text_color"){
            continue;
        }
        str.erase(remove(str.begin(), str.end(), '"'), str.end());
        vector<string> ligne_vector = string_to_vector(str, ',');

//        Station(unsigned int p_id, const std::string & p_nom, const std::string & p_description,const Coordonnees & p_coords);
//        cout << typeid(stod(frais[4])).name() << endl;
//        Coordonnees coord = Coordonnees(frais[3], frais[4]);
//        double ticail = frais[3];

        if(ligne_vector[7]=="97BF0D"){
            Ligne ligne = Ligne(stoul(ligne_vector[0]), ligne_vector[2], ligne_vector[4], CategorieBus::COUCHE_TARD);
            m_lignes[stoul(ligne_vector[0])] = ligne;
//            cout << ligne << endl;
        } else if(ligne_vector[7]=="013888"){
            Ligne ligne = Ligne(stoul(ligne_vector[0]), ligne_vector[2], ligne_vector[4], CategorieBus::LEBUS);
            m_lignes[stoul(ligne_vector[0])] = ligne;
//            cout << ligne << endl;
        } else if(ligne_vector[7]=="E04503"){
            Ligne ligne = Ligne(stoul(ligne_vector[0]), ligne_vector[2], ligne_vector[4], CategorieBus::EXPRESS);
            m_lignes[stoul(ligne_vector[0])] = ligne;
//            cout << ligne << endl;
        } else {
            Ligne ligne = Ligne(stoul(ligne_vector[0]), ligne_vector[2], ligne_vector[4], CategorieBus::COUCHE_TARD);
            m_lignes[stoul(ligne_vector[0])] = ligne;
//            cout << ligne << endl;
        }

//        Ligne ligne = Ligne(stoul(ligne_vector[0]), ligne_vector[2], ligne_vector[4], my_map.find(ligne_vector[7])->second);
//        cout << ligne << endl;
    }

}

//! \brief ajoute les stations dans l'objet GTFS
//! \param[in] p_nomFichier: le nom du fichier contenant les station
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterStations(const std::string &p_nomFichier)
{

    ostringstream nomDufichier;

    nomDufichier << "../" << p_nomFichier;

    std::ifstream file(nomDufichier.str());
    std::string str;

    while(std::getline(file, str)){
        if(str == "stop_id,stop_name,stop_desc,stop_lat,stop_lon,stop_url,location_type,wheelchair_boarding"){
            continue;
        }
        str.erase(remove(str.begin(), str.end(), '"'), str.end());
//        cout << str << endl;
        vector<string> frais = string_to_vector(str, ',');

//        Station(unsigned int p_id, const std::string & p_nom, const std::string & p_description,const Coordonnees & p_coords);
//        cout << typeid(stod(frais[4])).name() << endl;
//        Coordonnees coord = Coordonnees(frais[3], frais[4]);
//        double ticail = frais[3];
        Station station = Station(stoul(frais[0]), frais[1], frais[2], Coordonnees(stod(frais[3]),stod(frais[4])));
        m_stations[stoul(frais[0])] = station;

//        cout << station << endl;
    }
}

//! \brief ajoute les transferts dans l'objet GTFS
//! \breif Cette méthode doit âtre utilisée uniquement après que tous les arrêts ont été ajoutés
//! \brief les transferts (entre stations) ajoutés sont uniquement ceux pour lesquelles les stations sont prensentes dans l'objet GTFS
//! \brief les transferts sont ajoutés dans m_transferts
//! \brief les from_station_id des stations de transfert sont ajoutés dans m_stationsDeTransfert
//! \param[in] p_nomFichier: le nom du fichier contenant les station
//! \throws logic_error si un problème survient avec la lecture du fichier
//! \throws logic_error si tous les arrets de la date et de l'intervalle n'ont pas été ajoutés
void DonneesGTFS::ajouterTransferts(const std::string &p_nomFichier)
{

//écrire votre code ici

}


//! \brief ajoute les services de la date du GTFS (m_date)
//! \param[in] p_nomFichier: le nom du fichier contenant les services
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterServices(const std::string &p_nomFichier)
{



}

//! \brief ajoute les voyages de la date
//! \brief seuls les voyages dont le service est présent dans l'objet GTFS sont ajoutés
//! \param[in] p_nomFichier: le nom du fichier contenant les voyages
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterVoyagesDeLaDate(const std::string &p_nomFichier)
{

//écrire votre code ici

}

//! \brief ajoute les arrets aux voyages présents dans le GTFS si l'heure du voyage appartient à l'intervalle de temps du GTFS
//! \brief De plus, on enlève les voyages qui n'ont pas d'arrêts dans l'intervalle de temps du GTFS
//! \brief De plus, on enlève les stations qui n'ont pas d'arrets dans l'intervalle de temps du GTFS
//! \param[in] p_nomFichier: le nom du fichier contenant les arrets
//! \post assigne m_tousLesArretsPresents à true
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterArretsDesVoyagesDeLaDate(const std::string &p_nomFichier)
{

//écrire votre code ici

}



