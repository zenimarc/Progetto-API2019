# Prova Finale di Algoritmi e Strutture Dati -  a.a. 2018-2019
  Scopo del progetto è quello di implementare in linguaggio C standard un sistema in grado di gestire __entità__ e __relazioni__ . 
  
  L'istanza da gestire è fornita in un file di testo ricevuto come standard input, nel quale vengono definite le __entità__ da monitorare e le __relazioni__ tra queste.
  
  Attraverso i seguenti comandi:
  
  * addent
  * addrel
  * delent
  * delrel
  * report
  * end
  
  vengono istanziate nuove entità o relazioni (__addent__ e __addrel__) ed eliminate (__delent__ e __delrel__); il comando __report__ serve per verificare la corretta gestione del caso fornito.
  
  La __report__ scrive su standard output, in ordine alfabetico, tutte le relazioni che sta gestendo il sistema fornendo per ciascuna di queste, l'entità che riceve il maggior numero di quella relazione; in caso più entità ricevessero lo stesso numero di una relazione verrebbero considerate tutte in ordine alfabetico prima del numero di occorrenze.
  
