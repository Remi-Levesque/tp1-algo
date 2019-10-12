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


//! \brief ajoute les lignes dans l'objet GTFS
//! \param[in] p_nomFichier: le nom du fichier contenant les lignes
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterLignes(const std::string &p_nomFichier)
{

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



        if(ligne_vector[7]=="97BF0D"){
            Ligne ligne = Ligne(stoul(ligne_vector[0]), ligne_vector[2], ligne_vector[4], CategorieBus::COUCHE_TARD);
            m_lignes.insert(pair <unsigned int, Ligne> (stoul(ligne_vector[0]), ligne));
            m_lignes_par_numero.insert(pair <string, Ligne> (ligne_vector[2], ligne));
        } else if(ligne_vector[7]=="013888"){
            Ligne ligne = Ligne(stoul(ligne_vector[0]), ligne_vector[2], ligne_vector[4], CategorieBus::LEBUS);
            m_lignes.insert(pair <unsigned int, Ligne> (stoul(ligne_vector[0]), ligne));
            m_lignes_par_numero.insert(pair <string, Ligne> (ligne_vector[2], ligne));
        } else if(ligne_vector[7]=="E04503"){
            Ligne ligne = Ligne(stoul(ligne_vector[0]), ligne_vector[2], ligne_vector[4], CategorieBus::EXPRESS);
            m_lignes.insert(pair <unsigned int, Ligne> (stoul(ligne_vector[0]), ligne));
            m_lignes_par_numero.insert(pair <string, Ligne> (ligne_vector[2], ligne));
        } else {
            Ligne ligne = Ligne(stoul(ligne_vector[0]), ligne_vector[2], ligne_vector[4], CategorieBus::COUCHE_TARD);
            m_lignes.insert(pair <unsigned int, Ligne> (stoul(ligne_vector[0]), ligne));
            m_lignes_par_numero.insert(pair <string, Ligne> (ligne_vector[2], ligne));
        }

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
        vector<string> frais = string_to_vector(str, ',');

        Station station = Station(stoul(frais[0]), frais[1], frais[2], Coordonnees(stod(frais[3]),stod(frais[4])));
        m_stations[stoul(frais[0])] = station;

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
void DonneesGTFS::ajouterTransferts(const std::string &p_nomFichier) {

    ostringstream nomDufichier;

    nomDufichier << "../" << p_nomFichier;

    std::ifstream file(nomDufichier.str());
    std::string str;

    while (std::getline(file, str)) {
        if (str == "from_stop_id,to_stop_id,transfer_type,min_transfer_time") {
            continue;
        }

        vector<string> frais = string_to_vector(str, ',');

        if(frais[1] == "to_stop_id"){

            continue;
        }

        if(m_stations.count(stoul(frais[0])) and m_stations.count(stoul(frais[1])) ){
            if(stoul(frais[3]) == 0){
                m_transferts.push_back(tuple<unsigned int, unsigned int, unsigned int> (stoul(frais[0]), stoul(frais[1]), 1));
            } else {
                m_transferts.push_back(tuple<unsigned int, unsigned int, unsigned int> (stoul(frais[0]), stoul(frais[1]), stoul(frais[3])));
            }

            m_stationsDeTransfert.insert(stoul(frais[0]));

        }
    }
}


//! \brief ajoute les services de la date du GTFS (m_date)
//! \param[in] p_nomFichier: le nom du fichier contenant les services
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterServices(const std::string &p_nomFichier)
{

    ostringstream nomDufichier;

    nomDufichier << "../" << p_nomFichier;

    std::ifstream file(nomDufichier.str());
    std::string str;

    while(std::getline(file, str)) {
        if (str == "service_id,date,exception_type") {
            continue;
        }

        vector<string> frais = string_to_vector(str, ',');

//        Date(unsigned int an, unsigned int mois, unsigned int jour);

        Date date = Date(stoul(frais[1].substr(0,4)), stoul(frais[1].substr(4,2)), stoul(frais[1].substr(6,2)));

        if(frais[2] == "1" && date == m_date){
            m_services.insert(frais[0]);
        }

    }
}

//! \brief ajoute les voyages de la date
//! \brief seuls les voyages dont le service est présent dans l'objet GTFS sont ajoutés
//! \param[in] p_nomFichier: le nom du fichier contenant les voyages
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterVoyagesDeLaDate(const std::string &p_nomFichier)
{

    ostringstream nomDufichier;

    nomDufichier << "../" << p_nomFichier;

    std::ifstream file(nomDufichier.str());
    std::string str;


    while(std::getline(file, str)) {
        str.erase(remove(str.begin(), str.end(), '"'), str.end());

        if (str == "route_id,service_id,trip_id,trip_headsign,trip_short_name,direction_id,block_id,shape_id,wheelchair_accessible") {
            continue;
        }

        vector<string> frais = string_to_vector(str, ',');

        for (const auto& elem: m_services) { /// a voir pas sur que cest efficace tout ca
            if(frais[1] == elem){
                m_voyages[frais[2]] = Voyage(frais[2], stoul(frais[0]), frais[1], frais[3]);
            }
        }
    }
}

//! \brief ajoute les arrets aux voyages présents dans le GTFS si l'heure du voyage appartient à l'intervalle de temps du GTFS
//! \brief De plus, on enlève les voyages qui n'ont pas d'arrêts dans l'intervalle de temps du GTFS
//! \brief De plus, on enlève les stations qui n'ont pas d'arrets dans l'intervalle de temps du GTFS
//! \param[in] p_nomFichier: le nom du fichier contenant les arrets
//! \post assigne m_tousLesArretsPresents à true
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterArretsDesVoyagesDeLaDate(const std::string &p_nomFichier) {

    ostringstream nomDufichier;

    nomDufichier << "../" << p_nomFichier;

    std::ifstream file(nomDufichier.str());
    std::string str;

    while (std::getline(file, str)) {
        if (str == "trip_id,arrival_time,departure_time,stop_id,stop_sequence,pickup_type,drop_off_type") {
            continue;
        }

        vector<string> frais = string_to_vector(str, ',');



        if(m_voyages.find(frais[0]) != m_voyages.end()){ /// si on trouve le trip_id d'un voyage qui match avec un arret

            Heure heure_arrivee = Heure(stoi(frais[1].substr(0,2)), stoi(frais[1].substr(3,2)), stoi(frais[1].substr(6,2)));
            Heure heure_depart = Heure(stoi(frais[2].substr(0,2)), stoi(frais[2].substr(3,2)), stoi(frais[2].substr(6,2)));

            if(heure_depart >= m_now1 && heure_arrivee < m_now2){ /// si c'est entre les heures valides
                Arret::Ptr a_ptr = make_shared<Arret>(stoul(frais[3]), heure_arrivee, heure_depart, stoul(frais[4]), frais[0]);
                m_voyages[frais[0]].ajouterArret(a_ptr);
                m_stations[stoi(frais[3])].addArret(a_ptr);
                m_nbArrets += 1; /// wtf ca donne le nombre de voyages
            }
        }
    }

    map<string, Voyage> copy_voyages = m_voyages;


    for(auto &it: copy_voyages){
        /// si ya pas d'arret pour le voyage, on efface
        if(it.second.getNbArrets() == 0){
            m_voyages.erase(it.first);
        }
    }

    map<unsigned int, Station> copy_stations = m_stations;


    for(auto &it: copy_stations){
        if(it.second.getNbArrets() == 0){
            m_stations.erase(it.first);
        }
    }

    m_tousLesArretsPresents = true;



    }






