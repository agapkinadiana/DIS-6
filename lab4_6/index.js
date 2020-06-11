const axios = require('axios');
const Logger = require('./logs/Logger');

const config = require('./config.json');
const db = require('./db')(config.dataCenter.db);
const logger = new Logger();


// Before running the file, create database scheme named "RIS4_6"
logger.log('Data center start');
setInterval(retrieveResourcesData, config.dataCenter.refreshDataInterval);
setInterval(sendData, config.dataCenter.sendDataInterval);
setInterval(syncTime, config.dataCenter.syncTimeInterval);


function retrieveResourcesData() {     //получение
    const receivedData = {};
    const resourceRequests = [];
    for (let i = 0; i < config.resource.count; i++) {
        resourceRequests.push(axios.get(`http://${config.resource.host}:${config.resource.portStartsFrom + i}/data`));
    }
    axios.all(resourceRequests)
        .then(responses => handleResponsesCollected(responses, receivedData))
        .then(() => pushData(receivedData))
        .catch(err => logger.log('Error while fetching data from resource: ' + err.toString()));
}

function handleResponsesCollected(responses, receivedData) {
    responses.forEach((response, i) => {
        if (response.status === 200 && response.data.status === 'open') {
            logger.log(`Collected data from ${response.data.name}`);
            receivedData[response.data.name] = response.data;
        } else {
            logger.log(`Resource-${i + 1} responded with the http status: ${response.status} and self-status: ${response.data.status}`);
        }
    });
}

function handleResponsesSent(responses) {
    responses.forEach(response => {
        console.log(JSON.stringify(response.data));
    });
}

function pushData(receivedData) {
    Object.keys(receivedData).forEach(resourceName => {
        db.DataEntries.bulkCreate(receivedData[resourceName].data, { //пересылаем данные в бд
            // If preferred to load data from several sources then check it off because there will appear same primary keys
            updateOnDuplicate: ['id']
        });
    });
}

function sendData() {
    const temporaryValues = [
        {
            id: 10,
            field1: 999,
            field2: '999'
        },
        {
            id: 11,
            field1: 9999,
            field2: '9999'
        },
        {
            id: 12,
            field1: 99999,
            field2: '99999'
        },
        {
            id: 13,
            field1: 999999,
            field2: '999999'
        },
        {
            id: 14,
            field1: 9999999,
            field2: '9999999'
        }
    ];

    const resourceRequests = [];
    for (let i = 0; i < config.resource.count; i++) {
        resourceRequests.push(axios.post(
            `http://${config.resource.host}:${config.resource.portStartsFrom + i}/data`,
            temporaryValues
        ));
    }
    axios.all(resourceRequests)
        .then(responses => handleResponsesSent(responses));
}

function syncTime() {
    const syncRequests = [];
    for (let i = 0; i < config.resource.count; i++) {
        syncRequests.push(axios.post(
            `http://${config.resource.host}:${config.resource.portStartsFrom + i}/sync`,
            {time: new Date()}
        ));
    }
    axios.all(syncRequests);
}
