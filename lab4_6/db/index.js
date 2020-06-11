const Sequelize = require('sequelize');

module.exports = (dbConfig, resourceCount = '', initData) => {

    const sequelize = new Sequelize(
        dbConfig.db_name + resourceCount.toString(),
        dbConfig.username,
        dbConfig.password,
        dbConfig.options
    );

    const dataEntries = require('./data-entries')(Sequelize, sequelize);

    sequelize.sync({force: true})
        .then(() => {
            if (initData) {
                dataEntries.bulkCreate(initData);
            }
        });

    return {
        DataEntries: dataEntries,

        Sequelize: Sequelize,
        sequelize: sequelize
    };
};
