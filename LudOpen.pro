#-------------------------------------------------
#
# Project created by QtCreator 2012-05-04T12:10:40
#
#-------------------------------------------------
#DEFINES   = QT_NO_DEBUG_OUTPUT\
#            QT_NO_WARNING_OUTPUT
QT        = widgets sql network core printsupport
TARGET    = LudOpen
TEMPLATE  = app
RESOURCES = ImagesIcones.qrc
# Si je veux que l'application démarre dans une fenêtre de shell
# pour avoir les messages d'erreur
#CONFIG   += CONSOLE

SOURCES += \
    main.cpp \
    f_statistiques.cpp \
    f_retour.cpp \
    f_reservation.cpp \
    f_recherchemembres.cpp \
    f_plusdestatistiques.cpp \
    f_paiement.cpp \
    f_membres.cpp \
    f_mainwindow.cpp \
    f_listejeux.cpp \
    f_jeux.cpp \
    f_historiquejeux.cpp \
    f_emprunt.cpp \
    f_detailsjeux.cpp \
    f_declarerintervention.cpp \
    f_ajoutsuppmodifjeux.cpp \
    f_ajoutsuppmodiffournisseurediteurs.cpp \
    f_ajoutercoticarte.cpp \
    f_abonnements.cpp \
    f_preferences.cpp \
    f_retards.cpp \
    Courriel.cpp \
    f_listemembres.cpp \
    f_popupcode.cpp \
    f_postit.cpp \
    f_popupclesttem.cpp \
    f_clesttem.cpp \
    f_imprimerfichecompletejeu.cpp \
    f_imprimeretiquettejeu.cpp \
    f_listereservations.cpp \
    d_image.cpp \
    lb_image.cpp \
    acces_photos_http.cpp \
    d_ajoutsuppmodiffournisseursediteurs.cpp

HEADERS  += \
    f_statistiques.h \
    f_retour.h \
    f_reservation.h \
    f_recherchemembres.h \
    f_plusdestatistiques.h \
    f_paiement.h \
    f_membres.h \
    f_mainwindow.h \
    f_listejeux.h \
    f_jeux.h \
    f_historiquejeux.h \
    f_emprunt.h \
    f_detailsjeux.h \
    f_declarerintervention.h \
    f_ajoutsuppmodifjeux.h \
    f_ajoutsuppmodiffournisseursediteurs.h \
    f_ajoutercoticarte.h \
    f_abonnements.h \
    TitreType.h \
    f_preferences.h \
    f_retards.h \
    Courriel.h \
    f_listemembres.h \
    f_popupcode.h \
    f_postit.h \
    f_popupclesttem.h \
    f_clesttem.h \
    f_imprimerfichecompletejeu.h \
    f_imprimeretiquettejeu.h \
    f_listereservations.h \
    acces_photos_http.h \
    d_image.h \
    lb_image.h \
    d_ajoutsuppmodiffournisseursediteurs.h

FORMS    += \
    f_statistiques.ui \
    f_retour.ui \
    f_reservation.ui \
    f_recherchemembres.ui \
    f_plusdestatistiques.ui \
    f_paiement.ui \
    f_membres.ui \
    f_mainwindow.ui \
    f_listejeux.ui \
    f_jeux.ui \
    f_historiquejeux.ui \
    f_emprunt.ui \
    f_detailsjeux.ui \
    f_declarerintervention.ui \
    f_ajoutsuppmodifjeux.ui \
    f_ajoutsuppmodiffournisseursediteurs.ui \
    f_ajoutercoticarte.ui \
    f_abonnements.ui \
    f_preferences.ui \
    f_retards.ui \
    f_listemembres.ui \
    f_popupcode.ui \
    f_postit.ui \
    f_popupclesttem.ui \
    f_clesttem.ui \
    f_imprimerfichecompletejeu.ui \
    f_imprimeretiquettejeu.ui \
    f_listereservations.ui \
    d_image.ui \
    d_ajoutsuppmodiffournisseursediteurs.ui
