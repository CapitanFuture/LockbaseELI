

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <regex.h>

#include "MQTTClient.h"
#include "library.h"
#include "session_list.h"

/*
 * Global variable for holding the function pointer for the callback from driver
 */
ELIDrv2App  globalCallback;

#define ADDRESS     "tcp://localhost:1883"
#define CLIENT_ID   "Alice"

#define TOPIC       "channel"
#define QOS         2
#define TIMEOUT     1000L


MQTTClient client;
// MQTTClient_message pubmsg = MQTTClient_message_initializer;


// session list
node_t * sessions = NULL;

int mqtt_create() {
    return MQTTClient_create(&client, ADDRESS, CLIENT_ID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
}

void mqtt_destroy() {
    MQTTClient_destroy(&client);
}



int mqtt_connect() {
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    int rc ;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) // TODO: Hier kommt er nicht zurück??
    {
        printf("Failed to connect, return code %d\n", rc);
        return rc;
    }
    return rc;
}

int mqtt_disconnect() {
    return MQTTClient_disconnect(client, 1000);
}

int mqtt_publish(const char* payload) {

    MQTTClient_deliveryToken token;

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = (char*)payload;
    pubmsg.payloadlen = (int) strlen(payload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    int rc;
    if ((rc = MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to publish, return code %d\n", rc);
        return rc;
    }
    printf("Waiting for up to %d seconds for publication of %s\n"
           "on topic %s for client with ClientID: %s\n",
           (int) (TIMEOUT / 1000), "sss", TOPIC, CLIENT_ID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);
    return rc;
}

int mqtt_receive_msg() {
    char* topicName = NULL;
    int topicLen;
    MQTTClient_message* msg = NULL;
    char* test_topic = "heartbeat";


    int rc;


    rc = MQTTClient_subscribe(client, test_topic, 2);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to subscribe return code %d\n", rc);
        return rc;
    }


    if ((rc = MQTTClient_receive(client, &topicName, &topicLen, &msg, 5000L)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to receive, return code %d\n", rc);
        return rc;
    }

    if (topicName) {

        if (msg != NULL) {
            printf("Message received on topic %s is %.*s", topicName, msg->payloadlen, (char*)(msg->payload));
            MQTTClient_freeMessage(&msg);
        }
        MQTTClient_free(topicName);
    }
    else
        printf("No message received within timeout period\n");

    rc = MQTTClient_unsubscribe(client, test_topic);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to unsubscribe return code %d\n", rc);
        return rc;
    }

    return MQTTCLIENT_SUCCESS;
}

char* session_id_to_string(int session_id) {
    static char buf[9];
    sprintf(buf, "%08X", session_id);
    return buf;
}

int string_to_session_id(const char* sSessID) {
    return (int)strtol(sSessID, NULL, 16);
}

char* json_payload_create(const char* sSessID, const char* sText) {
    static char buf[200];
    sprintf(buf,
            u8"{ session_id : '%s', text : '%s' }",
            sSessID, sText);
    return buf;
}

/*
 *  LbwELI() is the constructor of the interface object, which is required to use the interface. It expects the
 *  application to identify itself by its licence and LbwELI revision number, which allows the driver to support
 *  different interface revisions and adjust its services to different types of installations (e.g. factory, locksmith
 *  or end user installation). Secondly the constructor expects a callback function pointer to launch the driver's
 *  responses to job requests (s.b.).
 *  In case the constructor supports the requested interface revision, it simply returns EOK. In case it does not,
 *  it returns EREV, followed by the best interface it supports:
 *       [ID:Error],[TXT:DrvELIRev]
 *   This allows the application to 'downgrade' to an older drivers capabilities. If the driver cannot start
 *   for ny other reason, the constructor returns EUNKNOWN.
 */

const char* ELICreate( const char* sLic, const char* sLbwELIRev, ELIDrv2App callback ) {
    printf("Lizenz: %s Revision: %s", sLic, sLbwELIRev);

    // seed random number generator with clock
    srand(clock());

    int ret = mqtt_create();
    if (ret != MQTTCLIENT_SUCCESS) {
        printf("mqtt_create() => %i\n", ret);
        return "EUNKNOWN";
    }

    /* check requested revision
    if (sLbwELIRev > "1.2") {
       return "EREV\n"
              u8"[ID:Error],[TXT:DrvELIRev]";
    }*/

    globalCallback = callback;

    return "EOK";
}

/*
 * ELIDestroy() is the destructor of the interface object, it will be called on application termination or when
 * the application detaches from the driver for other reasons. The application will call the destructor anyway,
 * even if the constructor did not returned EOK.
 */
void ELIDestroy() {
    mqtt_destroy();
}

/*
 * To retrieve global information about the driver the application will call the interface function DriverInfo()
 *
 * The function returns a text pointer containing definitions for the following variables (s.b.).
 * The returned text must be available for the application at least until the next call of an interface function.
 * The information returned by DriverInfo() is expected to be unchanged within the same driver revision.
 */

const char* ELIDriverInfo() {

    return u8"DriverRevision=0.9\n"         // required
           u8"LbwELIRevision=0.1.1837\n"    // required
           u8"Manufacturer = KMGmbH,\"de:Körtner & Muth GmbH\"\n" // required
           u8"Products = [ID:product 1],[LTL:product name 1];[ID: product 2],[LTL:product name 2]\n" // required
           u8"DriverAuthor = [LTL:CaptainFuture]\n" // optional
           u8"DriverCopyright = [LTL:Copyright CaptainFuture (c) 2018]\n" // optional
           u8"DriverUI = [LTL:Start DemoApp]\n" // optional
            ;
}

/*
 * ELIDriverUI() will receive the application's current session id and the id of the current active system as
 * parameters (s.b.). This variable is optional, by default no driver provided user interface will be supported.
 */

void ELIDriverUI(const char* SessID, const char* SID) {
    printf("Start Browser  SessID='%s'  SID='%s'\n", SessID, SID);
}

/*
 * The device capabilities and limitations are regarded to be product dependent.
 * They will be requested by a call to the ProductInfo() method
 *
 * The function returns a text pointer containing definitions for the following variables (s.b.).
 * The returned text must be available for the application at least until the next call of an interface function.
 * The information returned by ProductInfo() is expected to be unchanged within the same driver revision.
 */

const char* ELIProductInfo( const char* sProductID ) {
    return
            u8"ProgrammingTarget=0\n" // required
            u8"DeviceCapacity=INT\n"  // required
            u8"TimePeriodCapacity=INT\n"  // required
            u8"EventTypes =[ID:EventID0],[ID:Class],[LTL:Description0];" // required
            u8"[ID:EventID1],[ID:Class],[LTL:Description1];\n"

            // the following values are optional
            u8"OnlineSystem=0\n"
            u8"DefaultAccess=0\n"
            u8"AccessByNmbOfLockings=0\n"
            u8"AccessByFloatingPeriod=0\n"
            u8"TimePeriodRecurrence=[ID:RecIntID1],[ID:RecIntID2]\n"
            u8"EventUpdateInterval=[INT:60]\n"
            u8"AccessUpdateInterval=[INT:120]\n"
            ;
}

const char* ELISystemInfo( const char* sUsers ) {
    // send message to broker
    // connect();
    // publish(PAYLOAD);
    // wait for response (PAYLOAD)
    // disconnect();
    return
            u8"[ID:Sys1],[ID:ProductID],[TXT:Name],[ACLR],['0':disable|'1':enable]\n"
            u8"[ID:Sys2],[ID:ProductID],[TXT:Name],[ACLR],['0':disable|'1':enable]\n"
            u8",[ID:Product1],,[ACLR]\n"
            u8",[ID:Product2],,[ACLR]\n";
}

const char* ELIOpen( const char* sUserList, const char* sSystem, const char* sExtData) {

    int ret = mqtt_connect();
    if (ret != MQTTCLIENT_SUCCESS) {
        printf("mqtt_connect() => %i\n", ret);
        return "EUNKNOWN";
    }

    node_t * node = new_session(&sessions, sUserList, sSystem, sExtData);
    printf("Session %08X\n", node->session_id);

    const char* sSessID = session_id_to_string(node->session_id);
  //  mqtt_publish(json_payload_create(sSessID, "The Dark side of the moon..."));

    mqtt_receive_msg();
    return sSessID;
    // return "EOK";
}

const char* ELIClose( const char* sSessID ) {

    int session_id = string_to_session_id(sSessID);

    printf("remove session_id %08X\n", session_id);
    remove_session(&sessions, session_id);
    printf("removed session_id %08X\n", session_id);

    int ret = mqtt_disconnect();
    if (ret != MQTTCLIENT_SUCCESS) {
        printf("mqtt_disconnect() => %i\n", ret);
        return "EUNKNOWN";
    }
    printf("disconnected\n");

    return "EOK";
}




