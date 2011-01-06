<?php



/**
 * This class defines the structure of the 'cc_show_schedule' table.
 *
 *
 *
 * This map class is used by Propel to do runtime db structure discovery.
 * For example, the createSelectSql() method checks the type of a given column used in an
 * ORDER BY clause to know whether it needs to apply SQL to make the ORDER BY case-insensitive
 * (i.e. if it's a text column type).
 *
 * @package    propel.generator.campcaster.map
 */
class CcShowScheduleTableMap extends TableMap {

	/**
	 * The (dot-path) name of this class
	 */
	const CLASS_NAME = 'campcaster.map.CcShowScheduleTableMap';

	/**
	 * Initialize the table attributes, columns and validators
	 * Relations are not initialized by this method since they are lazy loaded
	 *
	 * @return     void
	 * @throws     PropelException
	 */
	public function initialize()
	{
	  // attributes
		$this->setName('cc_show_schedule');
		$this->setPhpName('CcShowSchedule');
		$this->setClassname('CcShowSchedule');
		$this->setPackage('campcaster');
		$this->setUseIdGenerator(true);
		$this->setPrimaryKeyMethodInfo('cc_show_schedule_id_seq');
		// columns
		$this->addPrimaryKey('ID', 'DbId', 'INTEGER', true, null, null);
		$this->addForeignKey('SHOW_ID', 'DbShowId', 'INTEGER', 'cc_show', 'ID', true, null, null);
		$this->addColumn('SHOW_DAY', 'DbShowDay', 'DATE', true, null, null);
		$this->addColumn('POSITION', 'DbPosition', 'INTEGER', false, null, null);
		$this->addColumn('GROUP_ID', 'DbGroupId', 'INTEGER', true, null, null);
		// validators
	} // initialize()

	/**
	 * Build the RelationMap objects for this table relationships
	 */
	public function buildRelations()
	{
    $this->addRelation('CcShow', 'CcShow', RelationMap::MANY_TO_ONE, array('show_id' => 'id', ), 'CASCADE', null);
	} // buildRelations()

} // CcShowScheduleTableMap
