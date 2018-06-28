# Islandora Managed Access 
Islandora module for managing temporary access to restricted objects

## Requirements
* [Islandora](https://github.com/Islandora/islandora)
* [Entity API](https://www.drupal.org/project/entity)

## Installation
Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.

## Introduction
The Islandora Managed Access module was created to fit a use case where certain Islandora objects should show up in collection browsing and search results, but require a user to register an account in order to view said object so that repository administrators may keep track of who is looking at it (similar in concept to an archival reading room). This may be due to intellectual property involved, or perhaps the object in question contains sensitive data (such as human subjects). Using the Islandora Managed Access module, you can manage users' access to specific objects by requiring them to log in to see it, and keep track of the data provided by those users through the administrative interface.

The module creates three new entity types, described below:

### Managed Access Profile
An MA Profile is a definition for an access management policy that can be applied to any number of objects. It contains metadata about the kind of access management to be applied, including information that should be displayed to the user when encountering a managed object, what IPs & IP ranges are allowed to view the managed object, how long newly registered users should have access to these objects, a contact email, whether registering users should be immediately active or need to be activated manually, etc. Each MA Profile creates a related Drupal role which can be granted its own set of permissions independently of previously existing roles.

### Managed Access Objects
An MA Object is an Islandora object that is being managed with an MA Profile. Creating and deleting MA Objects will apply and remove management from their specified PIDs respectively. This works recursively as well, so placing an MA policy on collections or hierarchal cmodels like newspapers or compound objects will apply the MA policy to all of the original object's children. Any children of the original object that should NOT be managed can be bypassed by entering their PID into the MA Object's PID Whitelist when creating it.

### Managed Access Users
An MA User is an entity containing data submitted by a user who is registering to see an MA Object. The MA User entity is separate from but related to the actual Drupal user that is created, and exists to hold metadata about the user that would be of interest to repository administrators.

## Usage
To make use of the Islandora Managed Access module, you must first go to `/admin/islandora/tools/managed_access` which gives you an administrative view of all MA Profiles, Objects and Users. Create an MA Profile by clicking the "Add managed access profile" link, and fill out the form with all of the information about the policy you want to apply to objects in your repository. Of special importance are the "Justification Information", "Usage Information" and "Contact Information" on the form, which will be displayed to users attempting to view objects managed by this profile. Additionally, you may provide an "Access Lifetime" integer which determines for how many days a newly created MA User will exist before being automatically deleted (or enter 0 for non-temporary accounts), and an "IP Whitelist" where you may enter a comma-separated list of IP addresses and IP ranges (in [CIDR notation](https://en.wikipedia.org/wiki/Classless_Inter-Domain_Routing)) which should bypass access management. Note that the "User active by default?" checkbox determines whether users registering to access objects managed by this policy will have immediate access (newly created user accounts are active) or if they should require manual mediation (newly created user accounts are blocked).

Once you have created an MA Profile, apply it to an Islandora object by clicking the "Managed Access Objects" tab and then following the "Add managed access profile" link. Filling out the following form will apply the selected MA Profile to the selected Islandora object PID. If the object is a collection or hierarchichal, the MA policy will cascade down the hierarchy and be automatically applied to all children. If there are any children that you do not want to be managed by the MA policy, add thier PIDs to the "PID Whitelist" section of the MA Object creation page. If you have multiple children you'd like to whitelist, separate their PIDs by commas.

Now that an Islandora object is managed by an MA Profile, any user who attempts to access it will get rerouted to a special page explaining that the object is under special restriction and why (provided by the MA Profile's "Justification Information"), how the object may be used (provided by the MA Profile's "Usage Information"), who to contact if you have further questions (provided by the MA Profile's "Contact Information"), as well as a link button that takes them to a registration page where they can create a new account that has the proper role for viewing the object. The only time users will not see this special page when accessing a managed object is if:

1. They are accessing the object from a whitelisted IP address or range
2. They are logged in as a user that has the specific user role required by the MA Policy managing the object
3. They have the Drupal "Administrator" role

If the user registered to see an object managed by a policy that has the "User active by default?" checkbox checked, they will immediately be able to log in to a newly created account with the proper role, allowing them to see the object without delay. If the "User active by default?" box is unchecked, the registered user will be given a message that their request for access will be reviewed. In order to "activate" these users, view their MA User account (found at `/admin/islandora/tools/managed_access/users`) and check the "Active?" checkbox and then save. When saving an MA User account as active when it previously was not, the system will automatically send the user an email telling them that they have been activated with information about how to log in to the system. When saving an MA User account as inactive when it previously was, the system will automatically send a "deactivation" message to the user telling them that their account has expired.

Administrators who need access to information about what users have registered to view managed objects can use the `/admin/islandora/tools/managed_access/users` admin menu, where all the metadata about MA Users is listed in a table.

Please note that MA Objects and Users require an MA Profile to point to, so you may not create MA Objects or Users without first creating an MA Profile.

## Maintainer/Sponsors
* [Bryan J. Brown](https://github.com/bryjbrown)
* [Favenzio Calvo](https://github.com/favenzio)
* [FSU Libraries](https://github.com/fsulib)
