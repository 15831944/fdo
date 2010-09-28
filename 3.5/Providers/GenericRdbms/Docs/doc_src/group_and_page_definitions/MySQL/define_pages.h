/*! \mainpage Introduction
<div class="mainpage">
<!-- This is the index.html page that the user first sees. -->

<p>The Feature Data Objects (FDO) API provides access to data in a 
data store. A provider is a specific implementation of the FDO API 
that provides access to data in a particular data store. The FDO 
Provider for MySQL provides FDO with access to a MySQL-based data store.
</p>

<p>MySQL 5.0.22 and later is supported. You can create, edit, and delete MySQL 
data stores and schemas, or you can access existing databases in MySQL that 
were created by other applications. The FDO Provider for MySQL supports native 
MySQL geometry, indexing, two-dimensional geometry (compatible with the OGC 
Simple Feature specificaton), and spatial query operations, but not 
transactions. Auto-generated properties are auto-incremented. MySQL uses a 
revision number for optimistic concurrency.
</p>

\note
If you cannot connect to your MySQL data source and you receive the error 
message, "Specified credentials are not valid or the provider is unable to 
establish a connection," copy the libmySQL.dll file into the \\FDO\\bin folder 
under the Autodesk product install folder to resolve the problem.

<p>
A MySQL schema can support the following:
</p>
<ul>
  <li>Inheritance
  <li>Multiple schemas
  <li>Object properties
  <li>Association properties
  <li>Schema overrrides
  <li>Auto ID generation
  <li>Null value constraints
  <li>Unique value constraints
  <li>Composite unique value constraints
  <li>Spatial contexts
  <li>These geometry types: point, line string, polygon,multi-point, multi-line 
  string, multi-polygon, curve string, curve polygon, multi-curve string, 
  multi-curve polygon, linear ring, line string segment, circular arc segment, 
  and ring.
</ul>

<p>When you create a MySQL schema, the following restrictions apply:
</p>
<ul>
  <li>A feature class must define or inherit at least one identity property.
  <li>You cannot specify default values for data properties.
  <li>Inclusive value range constraints are not supported.
  <li>Identity properties cannot be nullable.
  <li>Read-only identity properties must be autogenerated.
  <li>A feature class can have multiple geometric properties; main geometry is 
  not mandatory, but this attribute of the feature class indicates which 
  geometry property to use as the default for queries and rendering. HasMeasure 
  and HasElevation are supported. 
  <li>The maximum length of a string is 65,535 bytes.
  <li>For decimal properties, precision must be between 1 and 65 inclusive and 
  scale must be between 0 and 30 inclusive.
</ul>


<p>The FDO Provider for MySQL API provides custom commands that are
specifically designed to work with the FDO API. For example, using 
these commands, you can do the following:

</p>
<ul>
  <li> Gather information about a provider.</li>
  <li> Transmit client services exceptions.</li>
  <li> Get lists of accessible data stores.</li>
  <li> Create connection objects.</li>
  <li> Create and execute spatial queries.</li>
</ul>
<p>There is also support for spatial data types.
</p>

<p>The MySQL architecture supports different storage engines. Choose an 
engine as needed, depending on its characteristics and capabilities, such as the following: 
</p>
<ul>
  <li> MyISAM is a disk-based storage engine. It does not
    support transactions.</li>
  <li> InnoDB is a disk-based storage engine. It has full ACID transaction
    capability.</li>
  <li> Memory (Heap) is a storage engine utilizing only RAM. It
    is very fast.</li>
  <li> BDB is the Berkley DB storage engine. It supports transactions.</li>
  <li> NDB is the MySQL Cluster storage engine.</li>
  <li> MERGE is a variation of MyISAM. A MERGE table is a
    collection of identical MyISAM tables, which means that
    all tables have the same columns, column types, indexes, and so on.</li>
</ul>
<p>
For more information, see <i>The Essential FDO</i> 
and the <i>FDO Developer's Guide</i>.
</p>

<br>
</div>
*/



