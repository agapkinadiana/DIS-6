
module.exports = (Sequelize, sequelize) => {
    return sequelize.define('data_entries', {
        id: {
            type: Sequelize.INTEGER,
            autoIncrement: true,
            primaryKey: true
        },
        field1: {
            type: Sequelize.INTEGER,
            allowNull: false
        },
        field2: {
            type: Sequelize.STRING
        }
    });
};
