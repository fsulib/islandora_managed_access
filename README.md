# Islandora Managed Access 
Islandora module for managing temporary access to restricted objects

## Introduction
The Islandora Managed Access module was created to fit a use case where certain Islandora objects should show up in collection browsing and search results, but require a user to register an account in order to view said object so that repository administrators may keep track of who is looking at it (similar in concept to an archival reading room).

The module creates three new entity types, described below:

### Managed Access Profile
An MA Profile is a definition for an access management policy that can be applied to any number of objects. It contains metadata about the kind of access management to be applied, including information that should be displayed to the user when encountering a managed object, what IPs & IP ranges  are allowed to view the managed object, and how long newly registered users should have access to these objects.

### Managed Access Objects
An MA Object is an Islandora object that is being managed with an MA Profile. Creating and deleting MA Objects will apply and remove management from their specified PIDs respectively.

### Managed Access Users
An MA User is 


## Requirements

* [Islandora](https://github.com/Islandora/islandora)
* [Entity API](https://www.drupal.org/project/entity)

## Installation

Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.

## Usage

## Maintainer/Sponsors

* [Bryan J. Brown](https://github.com/bryjbrown)
* [Favenzio Calvo](https://github.com/favenzio)
* [FSU Libraries](https://github.com/fsulib)


