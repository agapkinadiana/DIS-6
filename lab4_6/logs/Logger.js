const fs = require('fs');

const DEFAULT_LOG_PATH = `${__dirname}/files/`;

class Logger {
    constructor(logPath, type = 'Data-Center') {
        if (!logPath) {
            logPath = DEFAULT_LOG_PATH
        }
        this.logPath = logPath + `${new Date().getTime()}_${type}.log`;
    }

    log(action) {
        const debugLine = `\n${new Date().toISOString()}: ${action}`;
        console.log(action);
        fs.appendFile(this.logPath, debugLine, err => {
            if (err) {
                console.log('Error while debugging');
            }
        })
    }
}

module.exports = Logger;
