#ifndef FDOSMPHOWNER_H
#define FDOSMPHOWNER_H		1
/*
 * Copyright (C) 2004-2006  Autodesk, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 2.1 of the GNU Lesser
 * General Public License as published by the Free Software Foundation.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */

#ifdef _WIN32
#pragma once
#endif

#include <Sm/Ph/DbObjectCollection.h>
#include <Sm/Ph/LockTypesCollection.h>
#include <Sm/Ph/Table.h>
#include <Sm/Ph/View.h>
#include <Sm/Ph/Synonym.h>
#include <Sm/Ph/SpatialContextCollection.h>
#include <Sm/Ph/SpatialContextGeom.h>
#include <Sm/Ph/CoordinateSystemCollection.h>

static const FdoStringP FDOSYS_OWNER = L"FDOSYS";

class FdoSmPhDatabase;
class FdoSmPhRdDbObjectReader;
class FdoSmPhRdViewReader;
class FdoSmPhRdCoordSysReader;
class FdoSmPhRdConstraintReader;
class FdoSmPhRdColumnReader;
class FdoSmPhRdBaseObjectReader;
class FdoSmPhRdSynonymReader;
class FdoSmPhRdTableJoin;
class FdoSmPhRdSpatialContextReader;
class FdoSmPhRdCoordSysReader;
class FdoSmPhRdViewRelationsObjectReader;

class FdoSmPhIndexLoader;
class FdoSmPhSynonymBaseLoader;

// This class represents an Owner (Physical Schema). The exact meaning
// of Owner depends on the Provider. For example, in the Oracle Provider
// it represents an Oracle schema (user).
//
// An owner can contain a number of database objects (tables, views, etc.).
class FdoSmPhOwner : public FdoSmPhDbElement
{
public:

    /// Constructs an instance of an Owner object.
    /// 
    /// Parameters:
    /// 	name: owner name
    /// 	pDatabase: database containing this owner.
    ///      elementState: determines whether this database object is new
    FdoSmPhOwner(
        FdoStringP name, 
        bool hasMetaSchema,
        const FdoSmPhDatabase* pDatabase,
   		FdoSchemaElementState elementState = FdoSchemaElementState_Added
    );

	~FdoSmPhOwner(void);

    /// Get/Set the owner's password
    void SetPassword( FdoStringP password );
    FdoStringP GetPassword();

    /// Get/Set whether the owner has MetaSchema tables.
    /// The Set function can only be called when this owner's
    /// element state is FdoSchemaElementState_Added.
    inline void SetHasMetaSchema( bool hasMetaSchema ) { mHasMetaSchema = hasMetaSchema; }
    inline bool GetHasMetaSchema() { return mHasMetaSchema; }

    // Returns true if spatial contexts defined in SC metadata table (f_spatialcontexts)
    virtual bool GetHasSCMetaSchema();

    // Returns true if classes are defined in class metadata table (f_classdefinition)
    virtual bool GetHasClassMetaSchema();

    // Returns true if properties are defined in property metadata table (f_attributedefinition)
    virtual bool GetHasAttrMetaSchema();

    // Returns true if association properties are defined in the metadata table (f_associationdefinition)
    virtual bool GetHasAssocMetaSchema();

    // Returns true if object properties are defined in the metadata table (f_attributedependencies)
    virtual bool GetHasObPropMetaSchema();

    // Returns true if Schema Attribute Dictionary items are defined in the metadata table (f_sad)
    virtual bool GetHasSADMetaSchema();

    // Returns true if Schema option is defined in the metadata table (f_schemaoptions)
    virtual bool GetHasSCOptionMetaSchema();

    // Returns true if options is defined in the metadata table (f_options)
    virtual bool GetHasOptionMetaSchema();

    // Returns true if Schema option is defined in the metadata table (f_schemainfo)
    virtual bool GetHasSCInfoMetaSchema();

    // Returns true if spatial contexts geometry defined in SC metadata table (f_spatialcontextgeom)
    virtual bool GetHasSCGeomInfoMetaSchema();

    // Returns true if spatial contexts group defined in SC metadata table (f_spatialcontextgroup)
    virtual bool GetHasSCGroupInfoMetaSchema();

    /// System database flag
	void SetIsSystem( bool IsSystem );
	bool GetIsSystem();

    /// Returns the MetaSchema version for this Owner.
    /// Returns 0.0 if this owner has no MetaSchema.
    double GetSchemaVersion();

    /// Returns the description of the owner from  the MetaSchema.
    /// Returns L"" if this owner has no MetaSchema or description
    /// is actually blank.
    virtual FdoString* GetDescription() const;

    /// Long Transaction and Locking methods set/get
	void SetLtMode( FdoLtLockModeType LtMode );
	FdoLtLockModeType  GetLtMode() const;

	void SetLckMode( FdoLtLockModeType LckMode );
	FdoLtLockModeType GetLckMode() const;

    /// Given a lock mode, return an array of all lock types supported for this mode.
    /// The list depends on the current RDBMS.
    /// size is set to the size of the returned array.
    /// NULL is returned when size is 0.
	virtual const FdoLockType* GetLockTypes( FdoLtLockModeType lckMode, FdoInt32& size);

    /// Get a database object ...

    /// as a read-only pointer (NULL if not found)
    const FdoSmPhDbObject* RefDbObject(FdoStringP dbObject) const;

    /// as a read-write smart pointer (NULL if not found)
    FdoSmPhDbObjectP FindDbObject(FdoStringP dbObject);

    /// as a read-write smart pointer (exception thrown if not found)
    FdoSmPhDbObjectP GetDbObject(FdoStringP dbObject);

    // Find the given object in the given owner (datastore) and database instance.
    FdoSmPhDbObjectP FindReferencedDbObject(FdoStringP dbObject, FdoStringP owner = L"", FdoStringP database = L"" );

    // Get a currently cached database object at the given 0-based index.
    // Returns NULL if the index is out of range.
    // This function does not add database objects to the cache.
    FdoSmPhDbObjectP GetCachedDbObject(FdoInt32 idx);

    // Gets the physical spatial contexts for this owner. 
    // it contains the spatial contexts reverse-engineered from the RDBMS.
    FdoSmPhSpatialContextsP GetSpatialContexts();

    // Given a Spatial Context id, return the spatial context.
    // Returns NULL if the spatial contexts does not exist.
    FdoSmPhSpatialContextP FindSpatialContext( FdoInt64 scId );

    // Gets all spatial context to geometric column relations for this owner. 
    // it reverse-engineers the relations from the RDBMS.
    FdoSmPhSpatialContextGeomsP GetSpatialContextGeoms();

    // Given database object and column name, returns the association to spatial context.
    // Returns NULL if the column is not geometric or has no associated spatial context.
    FdoSmPhSpatialContextGeomP FindSpatialContextGeom( FdoStringP dbObjectName, FdoStringP columnName );

    // Given a Spatial Reference ID, return the coordinate system info
    // Returns NULL if coordinate system not found.
    virtual FdoSmPhCoordinateSystemP FindCoordinateSystem( FdoInt64 srid );

    // Return the coordinate system info for the given coordinate system name.
    // Returns NULL if coordinate system not found.
    virtual FdoSmPhCoordinateSystemP FindCoordinateSystem( FdoStringP csName );

    // Return the coordinate system info for the given well-known text.
    // Returns NULL if coordinate system not found.
    virtual FdoSmPhCoordinateSystemP FindCoordinateSystemByWkt( FdoStringP wkt );

    // Adds the given coordinate system to this owner's cache.
    // Does nothing if the coordinate system is already cached.
    virtual void CacheCoordinateSystem( FdoSmPhCoordinateSystemP coordSys );

    // Reverse-engineers an FDO feature schema name from this datastore.
    // Default implementation returns datastore name prepended by "Fdo".
    // "Fdo" is prepended to prevent name conflict with special schema 
    // that exists in all datastores with metaschema. The special schema
    // holds the datastore description.
    virtual FdoStringP GetBestSchemaName() const;

    /// Convert the given database object name to one that is unique and provider-acceptable.
	FdoStringP UniqueDbObjectName( FdoStringP objectName );

    /// Make this owner the current owner
    virtual void SetCurrent() {}

    /// Create a reader to get all database objects for this owner.
    virtual FdoPtr<FdoSmPhRdDbObjectReader> CreateDbObjectReader( FdoStringP dbObject = L"") const = 0;

    /// Create a reader to get database objects this owner and object name list.
    virtual FdoPtr<FdoSmPhRdDbObjectReader> CreateDbObjectReader( FdoStringsP objectNames ) const;

    /// Create a reader to get all database objects for this join.
    virtual FdoPtr<FdoSmPhRdDbObjectReader> CreateDbObjectReader( FdoPtr<FdoSmPhRdTableJoin> join ) const;

    /// Create a reader to get derived objects for this owner.
    /// A derived object is one where the RDBMS data dictionary does not explicitly
    /// relate the object to its components (e.g. columns), but the object implicitly
    /// has the same components as its base object. A typical derived object is a 
    /// synonym. The body of DoLoadSpatialContexts provides more details on why these
    /// retrievals are performed.
    virtual FdoPtr<FdoSmPhRdDbObjectReader> CreateDerivedObjectReader( FdoStringP objectName = L"") const;

    /// Create a reader to get derived objects this owner and object name list.
    virtual FdoPtr<FdoSmPhRdDbObjectReader> CreateDerivedObjectReader( FdoStringsP objectNames ) const;

    /// Create a reader to get all views for this owner.
    virtual FdoPtr<FdoSmPhRdViewReader> CreateViewReader() const;

    /// Create a reader to get one or more coordinate system(s) for this owner.
    virtual FdoPtr<FdoSmPhRdCoordSysReader> CreateCoordSysReader( FdoStringP csysName = L"") const;

    /// Create a reader to get one or more coordinate system(s) for this owner.
    virtual FdoPtr<FdoSmPhRdCoordSysReader> CreateCoordSysReader( FdoInt64 srid ) const;

    /// Create a reader to get a constraint by name
    virtual FdoPtr<FdoSmPhRdConstraintReader> CreateConstraintReader( FdoStringP constraintName ) const = 0;
 
    /// Create a reader to get all constraints for this owner and this table.
    virtual FdoPtr<FdoSmPhRdConstraintReader> CreateConstraintReader( FdoStringP tableName, FdoStringP constraintType ) const = 0;

	/// Get reader to retrieve all spatial contexts for the connection (no metaschema).
	virtual FdoPtr<FdoSmPhRdSpatialContextReader> CreateRdSpatialContextReader();

	/// Get reader to retrieve all spatial contexts for a database object.
	virtual FdoPtr<FdoSmPhRdSpatialContextReader> CreateRdSpatialContextReader( FdoStringP dbObjectName );

	/// Get reader to retrieve all spatial contexts for a list of database objects.
	virtual FdoPtr<FdoSmPhRdSpatialContextReader> CreateRdSpatialContextReader( FdoStringsP objectNames );

    // Create a reader to get all foreign keys (ordered by foreign table) for this owner.
    // Default implementation returns NULL (not supported).
    virtual FdoPtr<FdoSmPhRdFkeyReader> CreateFkeyReader() const;

    /// Create a reader to get foreign keys for this owner and object name list.
    virtual FdoPtr<FdoSmPhRdFkeyReader> CreateFkeyReader(  FdoStringsP objectNames ) const;

    // Create a reader to get all indexes (ordered by table) for this owner
    // Default implementation returns NULL (not supported).
    virtual FdoPtr<FdoSmPhRdIndexReader> CreateIndexReader() const;

    /// Create a reader to get all indexes for this owner and object name list.
    virtual FdoPtr<FdoSmPhRdIndexReader> CreateIndexReader( FdoStringsP objectNames ) const;

    // Create a reader to get all primary keys (ordered by table) for this owner
    // Default implementation returns NULL (not supported).
    virtual FdoPtr<FdoSmPhRdPkeyReader> CreatePkeyReader() const;

    /// Create a reader to get primary keys for this owner and object name list.
    virtual FdoPtr<FdoSmPhRdPkeyReader> CreatePkeyReader(  FdoStringsP objectNames ) const;

    // Create a reader to get all primary keys for this join.
    // Default implementation returns NULL (not supported).
    virtual FdoPtr<FdoSmPhRdPkeyReader> CreatePkeyReader( FdoPtr<FdoSmPhRdTableJoin> join ) const;

    /// Create a reader to get all constraints for this owner and object name list.
    virtual FdoPtr<FdoSmPhRdConstraintReader> CreateConstraintReader( FdoStringsP objectNames, FdoStringP constraintType ) const;

    /// Create a reader to get all constraints for this owner and this join.
    virtual FdoPtr<FdoSmPhRdConstraintReader> CreateConstraintReader( FdoPtr<FdoSmPhRdTableJoin> join, FdoStringP constraintType ) const;

    // Create a reader to get all columns for this owner
    virtual FdoPtr<FdoSmPhRdColumnReader> CreateColumnReader() const;

    // Create a reader to get all columns for this owner and list of objects.
    virtual FdoPtr<FdoSmPhRdColumnReader> CreateColumnReader( FdoStringsP objectNames ) const;

    // Create a reader to get all columns for this owner and join
    virtual FdoPtr<FdoSmPhRdColumnReader> CreateColumnReader( FdoPtr<FdoSmPhRdTableJoin> join ) const;

    // Create a reader to get all base object references for this owner
    virtual FdoPtr<FdoSmPhRdBaseObjectReader> CreateBaseObjectReader() const;

    // Create a reader to get all relations in a view 
    virtual FdoPtr<FdoSmPhRdViewRelationsObjectReader> CreateViewRelationsObjectReader( FdoStringsP objectNames ) const;

    virtual FdoPtr<FdoSmPhRdBaseObjectReader> CreateBaseObjectReader( FdoStringsP objectNames ) const;

    // Create a reader to get all synonyms for this owner
    virtual FdoPtr<FdoSmPhRdSynonymReader> CreateSynonymReader() const;

    // Create a reader to get a particular synonym
    virtual FdoPtr<FdoSmPhRdSynonymReader> CreateSynonymReader( FdoStringP synonymName ) const;

    // Create a reader to get all synonyms in the synonymNames collection.
    virtual FdoPtr<FdoSmPhRdSynonymReader> CreateSynonymReader( FdoStringsP synonymNames ) const;

    // Create a lazy bulk loader for indexes
    virtual FdoPtr<FdoSmPhIndexLoader> CreateIndexLoader(
        FdoSmPhDbObjectsP dbObjects      // candidate dbObject list. Index will be loaded for requested
                                         // DbObject plus some of these candidates. 
    );

    /// Create a new table. Table is not posted to the datastore until its Commit() function
    /// is called.
    FdoSmPhTableP CreateTable(
        FdoStringP tableName,
        FdoStringP pkeyName = L""
    );

    /// Create a new view. View is not posted to the datastore until its Commit() function
    /// is called.
    FdoSmPhViewP CreateView( 
        FdoStringP viewName, 
        FdoStringP rootDatabase,
        FdoStringP rootOwner,
        FdoStringP rootObjectName
    );

    /// Create a new synonym. Synonym is not posted to the datastore until its Commit() function
    /// is called.
    FdoSmPhSynonymP CreateSynonym(
        FdoStringP synonymName,
        FdoSmPhDbObjectP rootObject
    );

    // read and cache all objects for this owner. This function providers the best performance
    // when all objects for the owner need to be accessed.
    //
    // Parameters:
    //  cacheComponents: if true, cache each object's components (primary key, foreign
    //  keys, indexes). if false, then caching for listing purposes only.
    FdoSmPhDbObjectsP CacheDbObjects( bool cacheComponents );

    /// Given a DbObject reader, add its current database object to 
    /// this owner's cache.
    FdoSmPhDbObjectP CacheDbObject(
        FdoPtr<FdoSmPhRdDbObjectReader> reader,
        bool bulkFetchComponents = true
            // false: caching for listing purposes only
            // true: components will also be cached.
    );

    /// Remove a database object from the cache.
    void DiscardDbObject( FdoSmPhDbObject* dbObject );

    // Add a table or view name to fetch candidates list
    void AddCandDbObject( FdoStringP objectName );

    // Remove a table or view name from fetch candidates list
    void RemoveCandDbObject( FdoStringP objectName );

    // Add a table or view name to the index fetch candidates list
    void AddCandIndex( FdoStringP objectName );

    // Remove a table or view name from the index fetch candidates list
    void RemoveCandIndex( FdoStringP objectName );

    // Cache the base object for the given synonym along with up to 50 other candidates.
    void CacheSynonymBases( FdoStringP synonymName );

    /// Gather all errors for this element and child elements into a chain of exceptions.
    /// Adds each error as an exception, to the given exception chain and returns
    /// the chain.
	//
    /// parameters:
    /// 	pFirstException: a chain of exceptions.
	virtual FdoSchemaExceptionP Errors2Exception( FdoSchemaException* pFirstException = NULL ) const;

    /// Remove this owner from its database.
    virtual void Discard();

    // Event that notifies this object that a commit has taken place.
    // Clears the not found object list.
    virtual void OnAfterCommit();

    /// Serialize the table to an XML file.
    /// Mainly for unit testing.
	virtual void XMLSerialize( FILE* xmlFp, int ref ) const;

	/// Get the function name that returns current database name in the format and case used in F_SCHEMAINFO
	virtual FdoString* GetDbNameClause(bool isEqual) = 0;

	/// Insert Meta class
	virtual void CreateMetaClass() = 0;

    // Cache the indexes for the given dbObject along with up to 50 other candidates.
    void CacheCandIndexes( FdoStringP objectName );

    // Gets whether primary keys are being bulk loaded
    bool GetBulkLoadPkeys();

    // Sets whether to bulk primary keys
    void SetBulkLoadPkeys( bool bulkLoad );

    // Gets whether foreign keys are being bulk loaded
    bool GetBulkLoadFkeys();

    // Sets whether to bulk primary keys
    void SetBulkLoadFkeys( bool bulkLoad );

    // Returns true if all DbObjects for this owner have been cached.
    bool GetAreAllDbObjectsCached();

protected:
    //Unused constructor needed only to build on Linux
    FdoSmPhOwner() {}

    /// Load the lists of lock types per locking mode
    virtual void LoadLckTypes( FdoLtLockModeType lockMode );

    /// Returns a pointer to the database object list
    FdoSmPhDbObjectsP GetDbObjects();

    /// Get the lists of lock types per locking mode.
    FdoSmPhLockTypesCollection* GetLockTypesCollection();

    /// table and view creation functions which each provider
    /// must implement
    virtual FdoSmPhDbObjectP NewTable(
		FdoStringP tableName, 
		FdoSchemaElementState elementState,
        FdoSmPhRdDbObjectReader* reader
	) = 0;

    virtual FdoSmPhDbObjectP NewView(
		FdoStringP viewName, 
        FdoStringP rootDatabase,
        FdoStringP rootOwner,
        FdoStringP rootObjectName,
		FdoSchemaElementState elementState,
        FdoSmPhRdDbObjectReader* reader
	) = 0;

    virtual FdoSmPhDbObjectP NewDbObject(
        FdoStringP objName,
        FdoSchemaElementState elementState,
        FdoPtr<FdoSmPhRdDbObjectReader> reader
    );

    virtual FdoSmPhDbObjectP NewSynonym(
        FdoStringP synonymName,
        FdoSmPhDbObjectP rootObject,
        FdoSchemaElementState elementState,
        FdoSmPhRdDbObjectReader* reader
    );

    /// Commit modifications to database objects.
    virtual void CommitChildren( bool isBeforeParent );

    /// Commit modifications to foreign keys.
    virtual void CommitFkeys( bool isBeforeParent );

    /// Reserve the given database object name. Prevents UniqueDbObjectName from generating 
    /// this name.
	//
    /// Parameters
    /// 	objectName - the name to reserve
    /// 	bCheckReserved
    /// 		true - reserve only if not already reserved.
    /// 		false - skip the "is reserved" check and just add the table name
    /// 			to the in-memory reserved list. Useful when we know it is not
    /// 			currently reserved and we want to avoid the overhead of the 
    /// 			reserved check.
	void ReserveDbObjectName( FdoStringP objectName, bool bCheckReserved = true );

    /// Returns true if the given database object name is reserved (one of the following is true):
    /// 	- the object has been reserved by ReserveDbObjectName
    /// 	- the object exists in this owner (datastore)
    /// 	- the object is referenced by the metaschema tables in the current datastore.
	virtual bool IsDbObjectNameReserved( FdoStringP objectName );

    // Fetches and caches the given object plus some of the objects in the fetch candidates list.
    // This improves performance by fetching multiple objects at once.
    // Note that the fetch is only performed if the given object is also on the candidates list.
    FdoSmPhDbObjectP CacheCandDbObjects( FdoStringP objectName );

    // Returns the number of object candidates to fetch in one operation (See CacheCandDbObjects). 
    virtual FdoInt32 GetCandFetchSize();

    // Reset all loaders to start at the first cached dbObject when looking for 
    // candidates to load.
    void ResetLoaders();

    // Change the fetch components status for the given dbObject
    void SetBulkFetchComponents( 
        FdoSmPhDbObjectP dbObject, 
        bool bulkFetchComponents 
            // false: This dbObject is being cached for listing purposes only. It can 
            // be skipped when looking for candidates for lazy loading components
            // (e.g.: indexes, base objects)
            // true: This dbObject is being cached for component retrieval as well. It is 
            // a candidate for lazy loading components. 
    );

protected:
    // Checks each DbObject in this owner and adds its base object (if any) to 
    // the Candidates list for the base object's owner.
    // This helps base object retrieval performance when the current owner contains
    // views on tables in a different owner. It causes the base objects to be 
    // bulk fetched.
    virtual void LoadBaseObjectCands();

    // Caches spatial context to geometric column relationships, and physical spatial contexts.
    // When dbObjectName is blank, retrieves all spatial contexts for this owner,
    // otherwise only the spatial contexts associated with the geometric columns
    // in the given db object are loaded.
    virtual void LoadSpatialContexts( FdoStringP dbObjectName = L"" );

private:
    /// Load Schema Information
    void LoadSchemaInfo();

    /// Load the long transaction and locking settings.
    void LoadLtLck();

    void DoLoadSpatialContexts( FdoStringP dbObjectName );

    // Creates list of dbObjects that are candidates for spatial context loading.
    // This allows bulk fetching of physical spatial context info when only partially
    // describing a schema. 
    // Returned list is empty if the given dbObject is not a candidate.
    FdoStringsP GetRdScCands( FdoStringP dbObjectName );

    // Caches the coordinate systems retrieved by the given reader.
    void LoadCoordinateSystems( FdoPtr<FdoSmPhRdCoordSysReader> rdr );

    // Gathers candidate tables for bulk loading indexes. A table is added to the candidates
    // list if its indexes are needed for reverse-engineering in the following two cases:
    //      - table has no primary key
    //      - table has multiple geometric columns.
    void LoadIndexTableCands();
    // Gathers candidate tables for a 3rd case:
    //      - a view is based on the table, but the view does not contain all of the 
    //        table's primary key columns.
    // This triggers a load of indexes for all tables that have dependent views. This
    // could be further refined to skip the table if the view contains all primary key
    // columns. However, views that don't contain all primary key columns, are probably
    // rare so this refinement would not likely help performance much.
    void LoadIndexRootTableCands();

    // Helper method to read and cache all objects for this owner.
    //
    // Parameters:
    //  cacheComponents: if true, cache each object's components (primary key, foreign
    //  keys, indexes).
    void ReadAndCacheDbObjects(bool cacheComponents);


    bool mDbObjectsCached;              // true if all db objects have been cached.
    bool mDbComponentsCached;           // true if all db components (columns and keys) have been cached.
	FdoSmPhDbObjectsP mDbObjects;       // collection of cached objects
    FdoDictionaryP mNotClassifiedObjects; // collection of object which were queried from the RDBMS but not
                                        // classified or do not exist. Use to prevent repeated attempts to fetch these objects.
	FdoStringsP mReservedDbObjectNames;
    FdoDictionaryP mCandDbObjects;      // List of candidate objects for fetching from RDBMS. 

    // Current indexes for next dbObject to check for base object bulk fetching.
    // Any dbobjects with lower index in the cache have already been checked.
    int mNextBaseCandIdx;

    // Current indexes for next dbObject to check for spatial context bulk fetching.
    // Any dbobjects with lower index in the cache have already been checked.
    int mNextRdScCandIdx;

    // Lazy bulk loader for indexes.
    FdoSmPhIndexLoader* mIndexLoader;

    // Lazy bulk loader for synonyms.
    FdoSmPhSynonymBaseLoader* mSynonymBaseLoader;

    // Cache of spatial contexts
    FdoSmPhSpatialContextsP mSpatialContexts;
    // Cache of spatial context to geometry column relationships
	FdoSmPhSpatialContextGeomsP mSpatialContextGeoms;
    bool mSpatialContextsLoaded;

    // Cache of coordinate systems.
    FdoSmPhCoordinateSystemsP mCoordinateSystems;
    bool mCoordinateSystemsLoaded;

    FdoStringP mPassword;
    bool mHasMetaSchema;
    /// FDOSYS database flag
	bool mIsSystem;

    double mSchemaVersion;

	FdoLtLockModeType mLtMode;
	FdoLtLockModeType mLckMode;
    FdoSmPhLockTypesCollectionP mLockTypes;
    bool mSchemaInfoLoaded;
    bool mLtLckLoaded;

    bool mBulkLoadPkeys;
    bool mBulkLoadFkeys;
    bool mHasOnlyFdoCand;

    // Statuses for cache candidates and mNotClassifiedObjects list
    static FdoString* NOT_CLASSIFIED;  // object exists but Schema Manager does not classify it (e.g. Oracle index)
    static FdoString* NOT_EXIST;       // object does not exist
    static FdoString* CLASSIFIED;      // object exists and can be classified by Schema Manager.
};

typedef FdoPtr<FdoSmPhOwner> FdoSmPhOwnerP;

#endif



