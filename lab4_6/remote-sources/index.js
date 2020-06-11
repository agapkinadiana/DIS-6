const express = require('express');
const bodyParser = require('body-parser');
const fs = require('fs');
const Logger = require('./../logs/Logger');
const config = require('./../config.json');


for (let i = 0; i < config.resource.count; i++) { //инициализируется сервер
    runResource(i);
}

// Before running the file, create two database schemes named "RIS4_6_Resource1" and "RIS4_6_Resource2"
function runResource(resourceCount) {
    const app = express();
    const port = config.resource.portStartsFrom + resourceCount;
    const stateFilePath = `${__dirname}/resource${resourceCount + 1}/state.json`;
    const sampleCopiesPath = `${__dirname}/sample-copies/state${resourceCount + 1}.json`;
    const logger = new Logger(undefined, `Resource-${resourceCount + 1}`);
    const db = require('./../db')(config.resource.db, resourceCount + 1);

    fs.copyFileSync(sampleCopiesPath, stateFilePath);

    app.use(bodyParser.json());

    app.get('/status', (request, response) => {
        fs.readFile(stateFilePath, (err, data) => {
            if (err) {
                logger.log(err.toString());
                response.status(400).json({error: err.toString()});
            } else {
                const state = JSON.parse(data.toString());
                logger.log(`Retrieving ${state.name}'s status: ${state.status}`);
                response.json({name: state.name, status: state.status});
            }
        });
    });

    app.get('/data', (request, response) => {
        fs.readFile(stateFilePath, (err, data) => {
            if (err) {
                logger.log(err.toString());
                response.status(400).json({error: err.toString()});
            } else {
                const state = JSON.parse(data.toString());
                logger.log(`Retrieving ${state.name}'s data, entity length: ${state.latestData.length}`);
                response.json({name: state.name, status: state.status, data: state.latestData});

                db.DataEntries.bulkCreate(state.latestData)
                    .catch(err => logger.log(`Error while committing old data for ${state.name}`));

                state.latestData = [];
                fs.writeFile(stateFilePath, JSON.stringify(state, null, '  '), err => {
                    if (err) {
                        logger.log(`Error while cleaning latest data from ${state.name}`);
                    }
                });
            }
        });
    });

    app.post('/data', (request, response) => {
        logger.log(`Retrieving data from data center for the ${resourceCount + 1} resource`);
        if (request.body && Array.isArray(request.body)) {
            const loadDataError = `Error while loading data from data center for the ${resourceCount + 1} resource`;
            db.DataEntries.bulkCreate(request.body, {
                updateOnDuplicate: ['id']
            }).then(() => {
                response.json({name: `resource-${resourceCount + 1}`});
            }).catch(err => {
                logger.log(loadDataError);
                logger.log(err.toString());
                response.json({error: loadDataError})
            });
        } else {
            const invalidDataError = `Got invalid data format from data center for the ${resourceCount + 1} resource`;
            logger.log(invalidDataError);
            response.status(400).json({error: invalidDataError});
        }
    });

    app.post('/sync', (request, response) => {
        const dataCenterTime = request.body.time;
        app.set('time', dataCenterTime);
        logger.log(`Synchronizing resource-${resourceCount + 1} time with the data center\'s one: ${dataCenterTime}`);
        response.end();
    });

    app.listen(port, () => {
        logger.log(`Running ${resourceCount} resource on http://${config.resource.host}:${port}`)
    });

    setInterval(produceNewData, config.resource.data.produceNewDataInterval, stateFilePath, logger);
}

function produceNewData(stateFilePath, logger) {
    logger.log(`Committing new data for ${stateFilePath}`);
    fs.readFile(stateFilePath, (err, data) => {
        if (err) {
            logger.log(`Error while fetching current state for ${stateFilePath}`);
        } else {
            const currentState = JSON.parse(data.toString());
            for (let i = 0; i < config.resource.data.producedDataCount; i++) {
                currentState.latestData.push(generateDataEntry());
            }
            fs.writeFile(stateFilePath, JSON.stringify(currentState, null, '  '), err => {
                if (err) {
                    logger.log(`Error while committing new data for for ${stateFilePath}`);
                }
            });
        }
    });
}

function generateDataEntry() {
    return {
        field1: Math.round(Math.random() * 100),
        field2: `${Math.round(Math.random() * 100)}`
    };
}
