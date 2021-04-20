Architecture of a generic service API which follows the Dependency Inversion Principle.
Details of the service are isolated from the client.

Assume we need to implement a service which has a few methods
available for the client, for example:

    call_service();

The service also has some state, hidden from the client.

To perform it's job, the service should call back the client
via a set of predefined methods, for example:

    call_client();

The client also has some state, hidden from the server.
