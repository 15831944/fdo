<?xml version="1.0" encoding="UTF-8" ?>
<!-- 
 
   Copyright (C) 2004-2006  Autodesk, Inc.
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of version 2.1 of the GNU Lesser
   General Public License as published by the Free Software Foundation.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
-->
<!-- This stylesheet converts a set of FDO Feature Schemas from 
  GML to Internal Format. The Internal format is an XML format that 
  mirrors the FDO Feature Schema structure.  -->
<stylesheet version="1.0" 
 xmlns="http://www.w3.org/1999/XSL/Transform"
 xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
 xmlns:xs="http://www.w3.org/2001/XMLSchema"
 xmlns:fdo="http://fdo.osgeo.org/schemas"
 xmlns:fdov2="http://www.autodesk.com/isd/fdo"
 xmlns:gml="http://www.opengis.net/gml"
 xmlns:wfs="http://www.opengis.net/wfs"
>
<!-- Stylesheet parameters -->
<!-- The URL prefix for the target namespace of each schema -->
<xsl:param name="customer_url" select="'fdo.osgeo.org/schemas/feature'"/>
<xsl:param name="schema_name_as_prefix" select="'no'"/>
<xsl:param name="element_default_nullability" select="'no'"/>
<xsl:param name="use_gml_id" select="'no'"/>
<xsl:param name="error_level" select="'n'"/>

  <!-- used for looking up cross-schema references -->
<xsl:key name="schemaKey" match="xs:schema" use="@targetNamespace" />
<xsl:key name="gblTypeKey" match="xs:schema/node()" use="concat(../@targetNamespace,':',@name)" />
<xsl:key name="gblElemTypeKey" match="xs:schema/xs:element" use="concat(../@targetNamespace,':',substring-after(@type,':'))" />
<xsl:key name="gblElemUTypeKey" match="xs:schema/xs:element" use="concat(../@targetNamespace,':',@type)" />
<xsl:key name="keyKey" match="xs:schema/node()/xs:key" use="concat(../../@targetNamespace,':',@name)" />
<xsl:key name="uniqueKey" match="xs:schema/node()/xs:unique" use="concat(../../@targetNamespace,':',@name)" />

<!-- Various constants -->
<xsl:variable name="gXsUri" select="'http://www.w3.org/2001/XMLSchema'"/>
<xsl:variable name="gGmlUri" select="'http://www.opengis.net/gml'"/>
<xsl:variable name="gFdoUri" select="'http://fdo.osgeo.org/schemas'"/>
<xsl:variable name="gFdoV2Uri" select="'http://www.autodesk.com/isd/fdo'"/>
<xsl:variable name="gWfsUri" select="'http://www.opengis.net/wfs'"/>
<xsl:variable name="gCustUriPrefix" select="concat('http://',$customer_url,'/')"/>
<xsl:variable name="gCustUriPrefixDefault" select="'http://fdo.osgeo.org/schemas/feature/'"/>
<xsl:variable name="gCustUriPrefixDefaultV2" select="'http://fdo_customer/'"/>
<xsl:variable name="gGmlSchemaElem" select="key('schemaKey',$gGmlUri)" />

  <xsl:template match="xs:schema" >
	<!-- get the schema name. -->
	<xsl:variable name="schemaName">
		<xsl:call-template name="uri_2_schema_name" >
			<xsl:with-param name="uri" >
				<xsl:value-of select="@targetNamespace" />
			</xsl:with-param>
		</xsl:call-template>
	</xsl:variable>
	<!-- Write the internal format schema element -->
	<xsl:element name="Schema">
		<xsl:attribute name="name" >
			<xsl:value-of select="$schemaName" />
		</xsl:attribute>
		<xsl:attribute name="targetNamespace" >
			<xsl:value-of select="@targetNamespace" />
		</xsl:attribute>
		<xsl:call-template name="schema_errors">
			<xsl:with-param name="schemaName" select="$schemaName"/>
		</xsl:call-template>
		<xsl:call-template name="element_subelements">
			<xsl:with-param name="schemaName" select="$schemaName"/>
		</xsl:call-template>
		<!-- validate each global element -->
		<xsl:for-each select="xs:element">
			<xsl:call-template name="element_global" >
				<xsl:with-param name="schemaName" >
					<xsl:value-of select="$schemaName" />
				</xsl:with-param>
			</xsl:call-template>
		</xsl:for-each>
		<!-- Convert each complex type to an FDO ClassDefinition -->
		<xsl:for-each select="xs:complexType">
			<xsl:call-template name="complexType" >
				<xsl:with-param name="schemaName" >
					<xsl:value-of select="$schemaName" />
				</xsl:with-param>
			</xsl:call-template>
		</xsl:for-each>
	</xsl:element> 
</xsl:template>

<xsl:template name="complexType" >
	<xsl:param name="schemaName" />

	<xsl:variable name="isRestriction">
	    <xsl:choose>
            <xsl:when test="xs:complexContent/xs:restriction">
                <xsl:value-of select="'yes'"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="'no'" />
            </xsl:otherwise>
	    </xsl:choose>
	</xsl:variable>
    <xsl:if test="$isRestriction='yes'">
        <!-- Todo: report errors in restrictions -->
    </xsl:if>
    <!-- resolve the base class -->
    <xsl:variable name="qBaseClass">
        <xsl:choose>
            <xsl:when test="$isRestriction='yes'">
                <xsl:value-of select="xs:complexContent/xs:restriction/@base" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="xs:complexContent/xs:extension/@base" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>
	<xsl:variable name="fullBaseClass">
		<xsl:call-template name="get_fullname" >
			<xsl:with-param name="typeName" select="$qBaseClass" />
		</xsl:call-template>			
	</xsl:variable>
	<xsl:variable name="baseUri"><xsl:value-of select="substring-before($fullBaseClass,' ')"/></xsl:variable>
	<xsl:variable name="baseName"><xsl:value-of select="substring-before(substring-after($fullBaseClass,' '),' ')"/></xsl:variable>
	<xsl:variable name="baseNs"><xsl:value-of select="substring-after(substring-after($fullBaseClass,' '),' ')"/></xsl:variable>

    <!-- get the root class type. This is the lowest level base class or
     the highest level base class not in the current document.
    -->
    <xsl:variable name="fullClassType">
        <xsl:call-template name="get_class_type" >
            <xsl:with-param name="schemaName" >
		        <xsl:value-of select="$schemaName" />
	        </xsl:with-param>
	        <xsl:with-param name="baseUri" >
		        <xsl:value-of select="ancestor::xs:schema/@targetNamespace" />
	        </xsl:with-param>
	        <xsl:with-param name="baseName" >
		        <xsl:value-of select="@name" />
	        </xsl:with-param>
	        <xsl:with-param name="restriction" select="xs:complexContent/xs:restriction" />
          <xsl:with-param name="typeElem" select="." />
        </xsl:call-template>
	</xsl:variable>

	<xsl:variable name="classType" select="substring-before($fullClassType,' ')" />
	<xsl:variable name="wkUri" select="substring-before(substring-after($fullClassType,' '),' ')" />
	<xsl:variable name="wkType" select="substring-before(substring-after(substring-after($fullClassType,' '),' '),' ')" />
	<xsl:variable name="rootUri" select="substring-before(substring-after(substring-after(substring-after($fullClassType,' '),' '),' '),' ')" />
	<xsl:variable name="rootType" select="substring-after(substring-after(substring-after(substring-after($fullClassType,' '),' '),' '),' ')" />

    <xsl:variable name="className">
        <xsl:call-template name="type_to_class">
            <xsl:with-param name="type" select="@name" />		                    
        </xsl:call-template>
    </xsl:variable>

 	<!-- Write out the class mapping element -->
    <xsl:element name="ClassMapping" >
	    <xsl:attribute name="name">
	        <xsl:value-of select="$className" />		                    
	    </xsl:attribute>
        <xsl:attribute name="gmlName">
	        <xsl:value-of select="@name" />		                    
       </xsl:attribute>				
        <xsl:if test="not($wkUri='')">
	        <xsl:attribute name="wkSchema">
                <xsl:call-template name="uri_2_schema_name">
                    <xsl:with-param name="uri" select="$wkUri" />
                </xsl:call-template>
	        </xsl:attribute>				
        </xsl:if>
        <xsl:if test="not($wkType='')">
            <xsl:attribute name="wkClass">
	            <xsl:call-template name="type_to_class">
                    <xsl:with-param name="type" select="$wkType" />		                    
	            </xsl:call-template>
   	        </xsl:attribute>				
   	    </xsl:if>
    </xsl:element>

 	<!-- Write out the class element -->
	<xsl:if test="(not($classType = 'Skip')) and ($isRestriction='no' or $classType='FeatureClass' or $classType='Class' or $classType='ClassDefinition')">
	    <xsl:element name="{$classType}" >
		    <xsl:attribute name="name">
   		        <xsl:value-of select="$className" />		                    
		    </xsl:attribute>
		    <xsl:if test="$baseName">
		        <!-- In FDO XML, restricted complexTypes are always
		            autogenerated. Therefore, set the base to the 
		            highest ancestor that is not restricted (restricted
		            complexTypes are skipped.
		        -->
	            <xsl:variable name="fullUnresBaseClass">
	                <xsl:choose>
	                    <xsl:when test="$classType = 'FeatureClass' or $classType = 'Class' or $classType = 'ClassDefinition'">
	                        <xsl:value-of select="concat($baseUri,' ',$baseName,' ')"/>
	                    </xsl:when>
	                    <xsl:otherwise>
		                    <xsl:call-template name="get_unrestricted_baseclass" >
		                        <xsl:with-param name="schemaName" select="$schemaName"/>
		                        <xsl:with-param name="baseUri" select="$baseUri"/>
		                        <xsl:with-param name="baseName" select="$baseName"/>
		                    </xsl:call-template>
	                    </xsl:otherwise>
	                </xsl:choose>
	            </xsl:variable>
            	<xsl:variable name="unresBaseUri"><xsl:value-of select="substring-before($fullUnresBaseClass,' ')"/></xsl:variable>
	            <xsl:variable name="unresBaseName"><xsl:value-of select="substring-before(substring-after($fullUnresBaseClass,' '),' ')"/></xsl:variable>
		        <!-- Check if base class is well-known. Relationships to well-known
		             base classes are not brought into FDO
		         -->
		        <xsl:variable name="baseWellKnown">
		            <xsl:choose>
		                <!-- Any FDO type is well-known, and is reflected on the ClassType
		                    of the FDO class definition
		                -->
		                <xsl:when test="$unresBaseUri = $gFdoUri or $unresBaseUri = $gFdoV2Uri">
		                    <xsl:value-of select="'yes'"/>
		                </xsl:when>
		                <!-- The any type is also well-known, FDO does not have an
		                    "any" type concept.
		                -->
		                <xsl:when test="$unresBaseUri=$gXsUri and $unresBaseName='any'">
		                    <xsl:value-of select="'yes'"/>
		                </xsl:when>
		                <!-- AbstractFeatureType (becomes a Feature Class) is 
		                     based on AbstractGMLType (becomes a Class). Discard
		                     this base type relationship since in FDO, a Feature
		                     Class cannot be based on a class.
		                -->
		                <xsl:when test="$unresBaseUri=$gGmlUri and $className = 'AbstractFeature'">
		                    <xsl:value-of select="'yes'"/>
		                </xsl:when>
		                <!-- If base is a GML type and the class type is known then this base
		                     is well know if it is not defined in the input document.
		                -->
		                <xsl:when test="$unresBaseUri=$gGmlUri and not($classType='ClassDefinition')">
                      <xsl:choose>
                        <xsl:when test="count($gGmlSchemaElem) = 0">
                          <!-- There is no GML schema so treat base as well known -->
                          <xsl:value-of select="'yes'"/>
                        </xsl:when>
                        <xsl:otherwise>
                          <xsl:variable name="elementElem" select="key('gblTypeKey',concat($unresBaseUri,':',$unresBaseName))" />
                          <xsl:choose>
                            <xsl:when test="count($elementElem) = 0">
                              <xsl:value-of select="'yes'"/>
                            </xsl:when>
                            <xsl:otherwise>
                              <xsl:value-of select="'no'"/>
                            </xsl:otherwise>
                          </xsl:choose>
                        </xsl:otherwise>
                      </xsl:choose>      
 		                </xsl:when>
		                <xsl:otherwise>
                            <xsl:value-of select="'no'"/>
		                </xsl:otherwise>
		            </xsl:choose>
		        </xsl:variable>

		        <!-- If class has base class that is not well-known, write base class and schema attributes -->
		        <xsl:if test="not($unresBaseName='' or $baseWellKnown='yes'  )" >
			        <xsl:attribute name="baseClass">
		                <xsl:call-template name="type_to_class">
    		                <xsl:with-param name="type" select="$unresBaseName" />		                    
		                </xsl:call-template>
			        </xsl:attribute>
			        <xsl:attribute name="baseSchema">
   	                    <xsl:call-template name="uri_2_schema_name">
		                    <xsl:with-param name="uri" select="$unresBaseUri" />
                        </xsl:call-template>
  			        </xsl:attribute>
			        <xsl:if test="$isRestriction='yes'">
			            <xsl:attribute name="restricted">true</xsl:attribute>
			        </xsl:if>
			        <!-- Might need root base class if exact class type is not yet known
			            this class is used to resolve class type on read.
			        -->
			        <xsl:if test="not($rootUri='')">
   				        <xsl:attribute name="rootSchema">
   	                        <xsl:call-template name="uri_2_schema_name">
                                <xsl:with-param name="uri" select="$rootUri" />
                            </xsl:call-template>
   				        </xsl:attribute>				
   			        </xsl:if>
   			        <xsl:if test="not($rootType='')">
   				        <xsl:attribute name="rootClass">
		                    <xsl:call-template name="type_to_class">
    			                <xsl:with-param name="type" select="$rootType" />		                    
		                    </xsl:call-template>
   				        </xsl:attribute>				
   			        </xsl:if>
		        </xsl:if>
		    </xsl:if>
		    <xsl:if test="@abstract='true'">
			    <xsl:attribute name="abstract">true</xsl:attribute>
		    </xsl:if>
		    <xsl:if test="@fdo:hasGeometry" >
			    <xsl:attribute name="hasGeometry" >
			        <xsl:value-of select="@fdo:hasGeometry" />
			    </xsl:attribute>
		    </xsl:if>
		    <xsl:if test="@fdov2:hasGeometry" >
			    <xsl:attribute name="hasGeometry" >
			        <xsl:value-of select="@fdov2:hasGeometry" />
			    </xsl:attribute>
		    </xsl:if>
		    <xsl:if test="@fdo:geometryName" >
			    <xsl:attribute name="geometryProperty" >
			        <xsl:value-of select="@fdo:geometryName" />
			    </xsl:attribute>
		    </xsl:if>
		    <xsl:if test="@fdov2:geometryName" >
			    <xsl:attribute name="geometryProperty" >
			        <xsl:value-of select="@fdov2:geometryName" />
			    </xsl:attribute>
		    </xsl:if>
		    <!-- network primitives have cost -->
		    <xsl:if test="@fdo:costElement" >
			    <xsl:attribute name="costProperty" >
			        <xsl:value-of select="@fdo:costElement" />
			    </xsl:attribute>
		    </xsl:if>
		    <xsl:if test="@fdov2:costElement" >
			    <xsl:attribute name="costProperty" >
			        <xsl:value-of select="@fdov2:costElement" />
			    </xsl:attribute>
		    </xsl:if>
		    <!-- Handle network layer -->
		    <xsl:if test="$classType = 'NetworkClass'">
		        <xsl:call-template name="network_attributes">
    		        <xsl:with-param name="schemaName" select="$schemaName"/>
		                <xsl:with-param name="baseUri" select="$baseUri"/>
		                <xsl:with-param name="baseClass" select="$baseName"/>
		        </xsl:call-template>
		    </xsl:if>
		    <xsl:call-template name="complexType_errors">
			    <xsl:with-param name="schemaName" select="$schemaName"/>
			    <xsl:with-param name="classType" select="$classType"/>
			    <xsl:with-param name="className" select="$className"/>
		    </xsl:call-template>
		    <xsl:call-template name="element_subelements" />
		    <!-- Get identity properties from corresponding global element, if any -->
		    <xsl:call-template name="element_ref">
			    <xsl:with-param name="schemaName" select="$schemaName" />
			    <xsl:with-param name="typeAbstract" select="@abstract" />
		    </xsl:call-template>
		    <!-- Convert sub-elements to class properties -->
		    <xsl:call-template name="ClassProperties" >
			    <xsl:with-param name="schemaName" select="$schemaName" />
		    </xsl:call-template>
		    <!-- Add special association sub-elements for various
		        network classes.
		    -->
		    <xsl:if test="$classType = 'NetworkLinkFeatureClass'">
		        <xsl:call-template name="link_subelements">
    		        <xsl:with-param name="schemaName" select="$schemaName" />
		            <xsl:with-param name="baseUri" select="$baseUri"/>
		            <xsl:with-param name="baseClass" select="$baseName"/>
		        </xsl:call-template>
		    </xsl:if>
		    <xsl:if test="$classType = 'NetworkNodeFeatureClass'">
		        <xsl:call-template name="node_subelements">
			        <xsl:with-param name="schemaName" select="$schemaName" />
    	            <xsl:with-param name="baseUri" select="$baseUri"/>
		            <xsl:with-param name="baseClass" select="$baseName"/>
		        </xsl:call-template>
		    </xsl:if>
	    </xsl:element>
	</xsl:if>
</xsl:template>

<xsl:template name="element_global" >
	<xsl:param name="schemaName" />
	<xsl:variable name="autogenElem">
	    <xsl:choose>
	        <xsl:when test="starts-with(@name,'AssociatedClass') or starts-with(@name,'NodeLayer_') or starts-with(@name,'DirectedNode_') or starts-with(@name,'ReferenceFeature_') or starts-with(@name,'ParentPrimitive_') or contains(@name,'Primitive') or @fdo:refType or @fdov2:refType">
	            <xsl:value-of select="'yes'"/>
	        </xsl:when>
	        <xsl:otherwise>
	            <xsl:value-of select="'no'"/>
	        </xsl:otherwise>
	    </xsl:choose>
	</xsl:variable>
    <xsl:variable name="fullType">
	    <xsl:call-template name="get_fullname" >
		    <xsl:with-param name="typeName" select="@type" />
	    </xsl:call-template>			
    </xsl:variable>
    <xsl:variable name="typeName"><xsl:value-of select="substring-before(substring-after($fullType,' '),' ')"/></xsl:variable>
    <xsl:variable name="typeUri"><xsl:value-of select="substring-before($fullType,' ')"/></xsl:variable>
	
	<!-- Write out the GML-FDO mappings for this element -->
	<xsl:call-template name="ElementMapping">
	    <xsl:with-param name="elementElem" select="."/>
	    <xsl:with-param name="typeUri" select="$typeUri"/>
	    <xsl:with-param name="typeName" select="$typeName"/>
  </xsl:call-template>

	<xsl:call-template name="element_global_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
		<xsl:with-param name="autogenElem" select="$autogenElem"/>
		<xsl:with-param name="typeUri" select="$typeUri"/>
		<xsl:with-param name="typeName" select="$typeName"/>
	</xsl:call-template>
</xsl:template>

<!-- the following finds the global element for the current complexType.
  If a global element is found, the identity properties are extracted from 
  it.
-->
<xsl:template name="element_ref">
	<xsl:param name="schemaName"/>
	<xsl:param name="typeAbstract" select="false" />
	
    <!-- get the URI for the current schema -->
    <xsl:variable name="schemaUri" select="ancestor::xs:schema/@targetNamespace" />
  
	<xsl:choose>
	    <xsl:when test="$use_gml_id = 'yes' and $schemaUri=$gGmlUri and @name='AbstractFeatureType'">
			<xsl:element name="IdentityProperties" >
			    <xsl:choose>
			        <xsl:when test="xs:attribute[@name='fid']">
        				<xsl:element name="IdentityProperty">fid</xsl:element>
			        </xsl:when>
			        <xsl:otherwise>
        				<xsl:element name="IdentityProperty">gml/id</xsl:element>
			        </xsl:otherwise>
			    </xsl:choose>
			</xsl:element>
	    </xsl:when>
	    <xsl:otherwise>
	        <xsl:variable name="typeLocalName" select="@name" />
	        <xsl:variable name="typeElem" select="."/>
          <xsl:variable name="elemKey" select="concat($schemaUri,':',$typeLocalName)" />
          <!--Find the global element in the current schema, whose type is the current complexType-->
          <xsl:variable name="globalElements" select="key('gblElemTypeKey',$elemKey)[xs:key]" />
        <xsl:choose>
          <xsl:when test="$globalElements">
            <xsl:call-template name="IdentityProperties">
              <xsl:with-param name="elements" select="$globalElements"/>
              <xsl:with-param name="schemaUri" select="$schemaUri"/>
              <xsl:with-param name="schemaName" select="$schemaName"/>
              <xsl:with-param name="typeElem" select="$typeElem"/>
              <xsl:with-param name="typeAbstract" select="$typeAbstract"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise>
            <xsl:variable name="globalUElements" select="key('gblElemUTypeKey',$typeLocalName)[xs:key]" />
            <xsl:call-template name="IdentityProperties">
              <xsl:with-param name="elements" select="$globalUElements"/>
              <xsl:with-param name="schemaUri" select="$schemaUri"/>
              <xsl:with-param name="schemaName" select="$schemaName"/>
              <xsl:with-param name="typeElem" select="$typeElem"/>
              <xsl:with-param name="typeAbstract" select="$typeAbstract"/>
            </xsl:call-template>

          </xsl:otherwise>
        </xsl:choose>
        
        
	    </xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template name="IdentityProperties">
  <xsl:param name="elements"/>
  <xsl:param name="schemaUri"/>
  <xsl:param name="schemaName"/>
  <xsl:param name="typeElem"/>
  <xsl:param name="typeAbstract"/>
  
  <xsl:for-each select="$elements" >
    <xsl:variable name="fullType">
      <xsl:call-template name="get_fullname" >
        <xsl:with-param name="typeName" select="@type" />
      </xsl:call-template>
    </xsl:variable>
    <xsl:if test="substring-before($fullType,' ') = $schemaUri">
      <!--One global element found, extract the identity properties-->
      <xsl:variable name="idProps" select="xs:key" />
      <xsl:call-template name="element_ref_errors">
        <xsl:with-param name="schemaName" select="$schemaName"/>
        <xsl:with-param name="typeAbstract" select="$typeAbstract"/>
        <xsl:with-param name="typeElem" select="$typeElem"/>
      </xsl:call-template>
      <xsl:for-each select="$idProps[position()=1]" >
        <xsl:element name="IdentityProperties" >
          <xsl:for-each select="xs:field">
            <xsl:element name="IdentityProperty" >
              <xsl:value-of select="@xpath" />
            </xsl:element>
          </xsl:for-each>
        </xsl:element>
      </xsl:for-each>

      <xsl:variable name="uniProps" select="xs:unique" />
      <xsl:if test="count($uniProps) &gt; 0">
        <xsl:element name="UniqueConstraints" >
          <xsl:for-each select="$uniProps" >
            <xsl:element name="UniqueConstraint" >
              <xsl:for-each select="xs:field">
                <xsl:element name="ConstraintProperty" >
                  <xsl:value-of select="@xpath" />
                </xsl:element>
              </xsl:for-each>
            </xsl:element>
          </xsl:for-each>
        </xsl:element>
      </xsl:if>
    </xsl:if>
  </xsl:for-each>
</xsl:template>
  
<!-- Converts each sub-element of the current complexType to an FDO property
  definition.
-->
<xsl:template name="ClassProperties">
	<xsl:param name="schemaName" />
	<xsl:if test="xs:complexContent or xs:simpleContent or xs:sequence or xs:attribute or @fdo:geometryName or @fdov2:geometryName">
        <!-- get the URI for the current schema -->
        <xsl:variable name="schemaUri" select="ancestor::xs:schema/@targetNamespace" />

		<xsl:element name="Properties">
		    <!-- Base class relationship from GML 3 AbstractFeatureType to AbstractGMLType
		         is not carried into FDO. Therefore, explicitly add gml:id attribute
		         inherited from AbstractGMLType. Note that if AbstractFeatureType has a 
		         fid attribute then it is a GML 2 type and gml:id must not be added.
		    -->
	        <xsl:if test="$schemaUri=$gGmlUri and @name='AbstractFeatureType' and not(xs:attribute[@name='fid'])">
	            <xsl:element name="DataProperty">
		            <xsl:attribute name="name">
		                <xsl:value-of select="concat($schemaName,'/id')"/>
		            </xsl:attribute>
		            <xsl:attribute name="dataType">string</xsl:attribute>
	                <xsl:attribute name="length">255</xsl:attribute>
	                <!-- feature id is optional unless it is also being
	                     used as the identity property.
	                -->
		            <xsl:attribute name="nullable" >
		                <xsl:choose>
		                    <xsl:when test="$use_gml_id = 'yes'">
		                        <xsl:value-of select="'false'"/>
		                    </xsl:when>
		                    <xsl:otherwise>
		                        <xsl:value-of select="'true'"/>
		                    </xsl:otherwise>
		                </xsl:choose>
		            </xsl:attribute>
		        </xsl:element>
	        </xsl:if>
			<!-- generate geometric property for class's geometry property. -->
			<xsl:call-template name="geometry_property" >
				<xsl:with-param name="schemaName" select="$schemaName" />
			</xsl:call-template>
			<xsl:call-template name="Properties">
				<xsl:with-param name="schemaName" select="$schemaName" />
			</xsl:call-template>
		</xsl:element>
	</xsl:if>
</xsl:template>

<xsl:template name="Properties">
	<xsl:param name="schemaName" />

	<!-- the following loops chase down the sub-element hierarchy,
	 looking for element definitions.
	-->
	<xsl:for-each select="xs:complexContent" >
		<xsl:call-template name="complexContent" >
			<xsl:with-param name="schemaName" select="$schemaName" />
		</xsl:call-template>
	</xsl:for-each>
	<xsl:for-each select="xs:simpleContent" >
		<xsl:call-template name="simpleContent" >
			<xsl:with-param name="schemaName" select="$schemaName" />
		</xsl:call-template>
	</xsl:for-each>
	<xsl:apply-templates select="xs:sequence|xs:choice|xs:all|xs:group" >
		<xsl:with-param name="schemaName" select="$schemaName" />
		<xsl:with-param name="minOccurs">1</xsl:with-param>
		<xsl:with-param name="maxOccurs">1</xsl:with-param>
	</xsl:apply-templates>
	<xsl:for-each select="xs:attribute" >
		<xsl:call-template name="attribute" >
			<xsl:with-param name="schemaName" select="$schemaName" />
		</xsl:call-template>
	</xsl:for-each>
	<xsl:for-each select="xs:attributeGroup" >
		<xsl:call-template name="attributeGroup" >
			<xsl:with-param name="schemaName" select="$schemaName" />
		</xsl:call-template>
	</xsl:for-each>
</xsl:template>

<!-- Searches for attributes and sub-elements and converts them to FDO properties -->
<xsl:template name="complexContent">
	<xsl:param name="schemaName" />
	<xsl:call-template name="complexContent_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
	<xsl:for-each select="xs:extension" >
		<xsl:call-template name="extension_restriction" >
			<xsl:with-param name="schemaName" select="$schemaName" />
		</xsl:call-template>
	</xsl:for-each>
	<xsl:for-each select="xs:restriction" >
		<xsl:call-template name="extension_restriction" >
			<xsl:with-param name="schemaName" select="$schemaName" />
		</xsl:call-template>
	</xsl:for-each>
</xsl:template>

<!-- Handles complexType with simpleContent. Creates single data property
     to contain the simple content.
 -->
<xsl:template name="simpleContent">
	<xsl:param name="schemaName" />
    <xsl:variable name="fullBase">
        <xsl:call-template name="get_fullname" >
            <xsl:with-param name="typeName" select="xs:extension/@base" />
        </xsl:call-template>			
    </xsl:variable>
    <xsl:variable name="baseUri"><xsl:value-of select="substring-before($fullBase,' ')"/></xsl:variable>
    <xsl:variable name="baseName"><xsl:value-of select="substring-before(substring-after($fullBase,' '),' ')"/></xsl:variable>
	<xsl:variable name="fdoType">
		<xsl:call-template name="get_fdo_type" >
			<xsl:with-param name="uriName" select="$baseUri" />
			<xsl:with-param name="typeName" select="$baseName" />
		</xsl:call-template>
	</xsl:variable>
	<xsl:variable name="isClass">
		<xsl:call-template name="fdo_type_is_class" >
			<xsl:with-param name="fdoType" select="$fdoType" />
		</xsl:call-template>
	</xsl:variable>
	<!--<xsl:call-template name="simpleContent_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/> 
	</xsl:call-template>-->
	<xsl:if test="not($fdoType='') and ($isClass='no')">
		<xsl:call-template name="DataProperty" >
			<xsl:with-param name="schemaName" select="$schemaName" />
			<xsl:with-param name="minOccurs" select="0" />
			<xsl:with-param name="elementElem" select=".." />
			<xsl:with-param name="dataType" select="$fdoType" />
		</xsl:call-template>
	</xsl:if>
</xsl:template>

<!-- Searches for attributes and sub-elements and converts them to FDO properties -->
<xsl:template name="extension_restriction">
	<xsl:param name="schemaName" />
	<xsl:call-template name="sequence_etc_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
	<xsl:apply-templates select="xs:sequence|xs:choice|xs:all|xs:group" >
		<xsl:with-param name="schemaName" select="$schemaName" />
		<xsl:with-param name="minOccurs">1</xsl:with-param>
		<xsl:with-param name="maxOccurs">1</xsl:with-param>
	</xsl:apply-templates>
	<xsl:for-each select="xs:attribute" >
		<xsl:call-template name="attribute" >
			<xsl:with-param name="schemaName" select="$schemaName" />
		</xsl:call-template>
	</xsl:for-each>
	<xsl:for-each select="xs:attributeGroup" >
		<xsl:call-template name="attributeGroup" >
			<xsl:with-param name="schemaName" select="$schemaName" />
		</xsl:call-template>
	</xsl:for-each>
</xsl:template>

  <!-- Searches for choice sub-elements and converts them to FDO properties -->
  <xsl:template match="xs:choice">
    <xsl:param name="schemaName" />
    <xsl:param name="minOccurs" />
    <xsl:param name="maxOccurs" />
    <xsl:param name="choiceName" />
	  <xsl:variable name="genChoiceName">
		  <xsl:choose>
			  <xsl:when test="not($choiceName='')">
				  <xsl:value-of select="concat($choiceName, '_1')"/>
			  </xsl:when>
			  <xsl:otherwise>
				  <xsl:value-of select="'choiceId'" />
			  </xsl:otherwise>
		  </xsl:choose>
	  </xsl:variable>
	  <xsl:variable name="nextMinOccurs">
      <xsl:call-template name="resolve_minOccurs" >
        <xsl:with-param name="minOccurs" select="$minOccurs"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="nextMaxOccurs">
      <xsl:call-template name="resolve_maxOccurs" >
        <xsl:with-param name="maxOccurs" select="$maxOccurs"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:call-template name="sequence_etc_errors">
      <xsl:with-param name="schemaName" select="$schemaName"/>
    </xsl:call-template>
    <xsl:apply-templates select="xs:sequence|xs:choice|xs:all|xs:group|xs:element" >
      <xsl:with-param name="schemaName" select="$schemaName" />
      <xsl:with-param name="maxOccurs" select="$nextMaxOccurs" />
      <xsl:with-param name="minOccurs" select="$nextMinOccurs" />
      <xsl:with-param name="choiceName" select="$genChoiceName" />
    </xsl:apply-templates>
  </xsl:template>

  <!-- Searches for attributes and sub-elements and converts them to FDO properties -->
<xsl:template match="xs:sequence|xs:all">
	<xsl:param name="schemaName" />
	<xsl:param name="minOccurs" />
	<xsl:param name="maxOccurs" />
  <xsl:param name="choiceName" />
	<xsl:variable name="nextMinOccurs">
		<xsl:call-template name="resolve_minOccurs" >
			<xsl:with-param name="minOccurs" select="$minOccurs"/>
		</xsl:call-template>
	</xsl:variable>
	<xsl:variable name="nextMaxOccurs">
		<xsl:call-template name="resolve_maxOccurs" >
			<xsl:with-param name="maxOccurs" select="$maxOccurs"/>
		</xsl:call-template>
	</xsl:variable>
	<xsl:call-template name="sequence_etc_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
	<xsl:apply-templates select="xs:sequence|xs:choice|xs:all|xs:group|xs:element" >
		<xsl:with-param name="schemaName" select="$schemaName" />
		<xsl:with-param name="maxOccurs" select="$nextMaxOccurs" />
		<xsl:with-param name="minOccurs" select="$nextMinOccurs" />
    <xsl:with-param name="choiceName" select="$choiceName" />
  </xsl:apply-templates>
<!--	<xsl:for-each select="xs:element" >
		<xsl:call-template name="element" >
			<xsl:with-param name="schemaName" select="$schemaName" />
			<xsl:with-param name="maxOccurs" select="$nextMaxOccurs" />
			<xsl:with-param name="minOccurs" select="$nextMinOccurs" />
		</xsl:call-template>
	</xsl:for-each>-->
</xsl:template>

<!-- Resolves a group reference and converts its sub-elements to FDO properties -->
<xsl:template match="xs:group">
	<xsl:param name="schemaName" />
	<xsl:param name="minOccurs" />
	<xsl:param name="maxOccurs" />
  <xsl:param name="choiceName" />
	<xsl:variable name="nextMinOccurs">
		<xsl:call-template name="resolve_minOccurs" >
			<xsl:with-param name="minOccurs" select="$minOccurs"/>
		</xsl:call-template>
	</xsl:variable>
	<xsl:variable name="nextMaxOccurs">
		<xsl:call-template name="resolve_maxOccurs" >
			<xsl:with-param name="maxOccurs" select="$maxOccurs"/>
		</xsl:call-template>
	</xsl:variable>
	<!-- Get the element for the referenced group -->
	<xsl:variable name="fullName">
		<xsl:call-template name="get_fullname" >
			<xsl:with-param name="typeName" select="@ref" />
		</xsl:call-template>			
	</xsl:variable>
	<xsl:variable name="uriName"><xsl:value-of select="substring-before($fullName,' ')"/></xsl:variable>
	<xsl:variable name="localName"><xsl:value-of select="substring-before(substring-after($fullName,' '),' ')"/></xsl:variable>
    <xsl:variable name="groupElem" select="key('gblTypeKey',concat($uriName,':',$localName))" />
	<!-- Convert each group sub-element to a property -->
	<xsl:for-each select="$groupElem" >
		<xsl:call-template name="sequence_etc_errors">
			<xsl:with-param name="schemaName" select="$schemaName"/>
		</xsl:call-template>
		<xsl:apply-templates select="xs:sequence|xs:choice|xs:all" >
			<xsl:with-param name="schemaName" select="$schemaName" />
			<xsl:with-param name="maxOccurs" select="$nextMaxOccurs" />
			<xsl:with-param name="minOccurs" select="$nextMinOccurs" />
      <xsl:with-param name="choiceName" select="$choiceName" />
		</xsl:apply-templates>
	</xsl:for-each>
</xsl:template>

<!-- Resolves an attribute group references and converts its attributes
     to properties
-->
<xsl:template name="attributeGroup">
	<xsl:param name="schemaName" />
	<!--<xsl:call-template name="attribute_group_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>-->
	<!-- Get the element for the referenced attribute group -->
	<xsl:variable name="fullName">
		<xsl:call-template name="get_fullname" >
			<xsl:with-param name="typeName" select="@ref" />
		</xsl:call-template>			
	</xsl:variable>
	<xsl:variable name="uriName"><xsl:value-of select="substring-before($fullName,' ')"/></xsl:variable>
	<xsl:variable name="localName"><xsl:value-of select="substring-before(substring-after($fullName,' '),' ')"/></xsl:variable>
    <xsl:variable name="groupElem" select="key('gblTypeKey',concat($uriName,':',$localName))" />
	<xsl:if test="not($groupElem='')">
        <xsl:for-each select="$groupElem/xs:attribute" >
            <xsl:call-template name="attribute" >
	            <xsl:with-param name="schemaName" select="$schemaName" />
            </xsl:call-template>
        </xsl:for-each>
        <xsl:for-each select="$groupElem/xs:attributeGroup" >
            <xsl:call-template name="attributeGroup" >
	            <xsl:with-param name="schemaName" select="$schemaName" />
            </xsl:call-template>
        </xsl:for-each>
	</xsl:if>
</xsl:template>

<!-- Converts an xs:element to a property -->
<xsl:template match="xs:element">
	<xsl:param name="schemaName" />
	<xsl:param name="minOccurs" />
	<xsl:param name="maxOccurs" />
  <xsl:param name="choiceName" />

	<xsl:variable name="nextMinOccurs">
		<xsl:call-template name="resolve_minOccurs" >
			<xsl:with-param name="minOccurs" select="$minOccurs"/>
		</xsl:call-template>
	</xsl:variable>
	<xsl:variable name="nextMaxOccurs">
		<xsl:call-template name="resolve_maxOccurs" >
			<xsl:with-param name="maxOccurs" select="$maxOccurs"/>
		</xsl:call-template>
	</xsl:variable>
	<xsl:call-template name="element_nonglobal_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
	<xsl:call-template name="element_attribute">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	    <xsl:with-param name="minOccurs" select="$nextMinOccurs"/>
	    <xsl:with-param name="maxOccurs" select="$nextMaxOccurs"/>
      <xsl:with-param name="choiceName" select="$choiceName" />
	</xsl:call-template>
</xsl:template>

<!-- Converts an xs:attribute to a property -->
<xsl:template name="attribute">
	<xsl:param name="schemaName" />
  <xsl:param name="choiceName" />
	<xsl:variable name="minOccurs">
	    <xsl:choose>
	        <xsl:when test="@use='required'">1</xsl:when>
	        <xsl:otherwise>0</xsl:otherwise>
	    </xsl:choose>
	</xsl:variable>
	<xsl:variable name="maxOccurs">
	    <xsl:choose>
	        <xsl:when test="@use='prohibited'">0</xsl:when>
	        <xsl:otherwise>1</xsl:otherwise>
	    </xsl:choose>
	</xsl:variable>
	<!--<xsl:call-template name="attribute_nonglobal_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>-->
	<xsl:call-template name="element_attribute">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	    <xsl:with-param name="minOccurs" select="$minOccurs"/>
	    <xsl:with-param name="maxOccurs" select="$maxOccurs"/>
      <xsl:with-param name="choiceName" select="$choiceName" />
	</xsl:call-template>
</xsl:template>

<!--Converts current element (xs:element or xs:attribute) to an FDO property -->
<xsl:template name="element_attribute">
	<xsl:param name="schemaName" />
	<xsl:param name="minOccurs" />
	<xsl:param name="maxOccurs" />
	<xsl:param name="elementElem" select="."/>
  <xsl:param name="choiceName" />
	<xsl:variable name="nextMinOccurs">
		<xsl:call-template name="resolve_minOccurs" >
			<xsl:with-param name="minOccurs" select="$minOccurs"/>
		</xsl:call-template>
	</xsl:variable>
	<xsl:variable name="nextMaxOccurs">
		<xsl:call-template name="resolve_maxOccurs" >
			<xsl:with-param name="maxOccurs" select="$maxOccurs"/>
		</xsl:call-template>
	</xsl:variable>
	<!-- Figure out the property type -->
	<xsl:choose>
		<xsl:when test="@ref" >
			<!-- This element references another element, follow the reference -->
			<!-- convert referenced element's qualified name to URI plus local name -->
			<xsl:variable name="fullName">
				<xsl:call-template name="get_fullname" >
					<xsl:with-param name="typeName" select="@ref" />
				</xsl:call-template>			
			</xsl:variable>
			<xsl:variable name="uriName"><xsl:value-of select="substring-before($fullName,' ')"/></xsl:variable>
			<xsl:variable name="localName"><xsl:value-of select="substring-before(substring-after($fullName,' '),' ')"/></xsl:variable>
	        <xsl:variable name="elemType">
		        <xsl:call-template name="get_element_fdo_type" >
			        <xsl:with-param name="uriName" select="$uriName" />
			        <xsl:with-param name="elemName" select="$localName" />
		        </xsl:call-template>
	        </xsl:variable>

	        <xsl:variable name="fdoType">
	            <xsl:call-template name="get_fdo_type" >
		            <xsl:with-param name="uriName" select="$uriName" />
		            <xsl:with-param name="typeName" select="$elemType" />
	            </xsl:call-template>
	        </xsl:variable>

			<xsl:choose>
				<xsl:when test="$fdoType = 'geometricProperty' or ($nextMaxOccurs = '1' and $fdoType = 'geometricAssociation')">
   					<!-- URI and local name indicate that this is a Geometric property -->
	                <xsl:call-template name="ElementMapping">
	                    <xsl:with-param name="elementElem" select="$elementElem"/>
	                    <xsl:with-param name="typeUri" select="$uriName"/>
	                    <xsl:with-param name="typeName" select="$elemType"/>
                      <xsl:with-param name="choiceName" select="$choiceName"/>
	                </xsl:call-template>

    				<xsl:call-template name="GeometricProperty" >
	    				<xsl:with-param name="schemaName" select="$schemaName" />
		    			<xsl:with-param name="elementElem" select="$elementElem"/>
			    		<xsl:with-param name="refUri" select="$uriName"/>
			    		<xsl:with-param name="refType" select="$localName"/>
			    		<xsl:with-param name="baseType" select="$localName"/>
				    </xsl:call-template>
				</xsl:when>
				<xsl:when test="$nextMaxOccurs = '1' and $fdoType = 'associationProperty'">
   					<!-- URI and local name indicate that this is an Association property -->
	                <xsl:call-template name="ElementMapping">
	                    <xsl:with-param name="elementElem" select="$elementElem"/>
	                    <xsl:with-param name="typeUri" select="$uriName"/>
	                    <xsl:with-param name="typeName" select="$elemType"/>
                      <xsl:with-param name="choiceName" select="$choiceName"/>
	                </xsl:call-template>

    				<xsl:call-template name="AssociationProperty" >
	    				<xsl:with-param name="schemaName" select="$schemaName" />
        				<xsl:with-param name="maxOccurs" select="$nextMaxOccurs" />
		    			<xsl:with-param name="elementElem" select="$elementElem"/>
			    		<xsl:with-param name="baseUri" select="$gGmlUri"/>
			    		<xsl:with-param name="baseType" select="'FeaturePropertyType'"/>
				    </xsl:call-template>
				</xsl:when>
				<xsl:otherwise>
				    <xsl:variable name="refElem" select="key('gblTypeKey',concat($uriName,':',$localName))" />
					<xsl:choose>
					    <xsl:when test="count($refElem) = 0">
					        <!-- Can't resolve reference, assume object property -->
	                        <xsl:call-template name="ElementMapping">
	                            <xsl:with-param name="elementElem" select="$elementElem"/>
	                            <xsl:with-param name="typeUri" select="$uriName"/>
	                            <xsl:with-param name="typeName" select="$localName"/>
                              <xsl:with-param name="choiceName" select="$choiceName"/>
	                        </xsl:call-template>

			                <xsl:call-template name="ObjectProperty" >
				                <xsl:with-param name="schemaName" select="$schemaName" />
				                <xsl:with-param name="maxOccurs" select="$nextMaxOccurs" />
				                <xsl:with-param name="elementElem" select="$elementElem"/>
				                <xsl:with-param name="refUri" select="$uriName" />
				                <xsl:with-param name="refType" select="$localName" />
			                </xsl:call-template>
					    </xsl:when>
					    <xsl:otherwise>
					        <!-- convert referenced element to a property -->
					        <xsl:for-each select="$refElem" >
						        <xsl:call-template name="element_attribute" >
							        <xsl:with-param name="schemaName" select="$schemaName" />
							        <xsl:with-param name="maxOccurs" select="$nextMaxOccurs" />
							        <xsl:with-param name="minOccurs" select="$nextMinOccurs" />
							        <xsl:with-param name="elementElem" select="$elementElem" />
                      <xsl:with-param name="choiceName" select="$choiceName" />
						        </xsl:call-template>
					        </xsl:for-each>			
					    </xsl:otherwise>
					</xsl:choose>
				</xsl:otherwise>
			</xsl:choose>
			<!-- Log an error when error level is high
			     since these references are not round-trip supported 
			-->
			<xsl:call-template name="unsupported_attribute" >
				<xsl:with-param name="schemaName" select="$schemaName" />
				<xsl:with-param name="attributeName" >ref</xsl:with-param>
				<xsl:with-param name="errorLevel" >h</xsl:with-param>
			</xsl:call-template>

		</xsl:when>
		<xsl:when test="xs:keyref">
			<!-- an element with a key reference is an Association Property -->
			<xsl:call-template name="AssociationProperty" >
				<xsl:with-param name="schemaName" select="$schemaName" />
				<xsl:with-param name="maxOccurs" select="$nextMaxOccurs" />
				<xsl:with-param name="elementElem" select="$elementElem" />
			</xsl:call-template>
		</xsl:when>
		<xsl:when test="@type" >
	        <xsl:variable name="fullType">
		        <xsl:call-template name="get_fullname" >
			        <xsl:with-param name="typeName" select="@type" />
		        </xsl:call-template>			
	        </xsl:variable>
	        <xsl:variable name="typeUri"><xsl:value-of select="substring-before($fullType,' ')"/></xsl:variable>
	        <xsl:variable name="typeName"><xsl:value-of select="substring-before(substring-after($fullType,' '),' ')"/></xsl:variable>

			<xsl:call-template name="element_type" >
				<xsl:with-param name="schemaName" select="$schemaName" />
				<xsl:with-param name="minOccurs" select="$nextMinOccurs" />
				<xsl:with-param name="maxOccurs" select="$nextMaxOccurs" />
				<xsl:with-param name="elementElem" select="$elementElem" />
				<xsl:with-param name="elemTypeUri" select="$typeUri" />
				<xsl:with-param name="elemTypeName" select="$typeName" />
				<xsl:with-param name="typeUri" select="$typeUri" />
				<xsl:with-param name="typeName" select="$typeName" />
        <xsl:with-param name="choiceName" select="$choiceName" />
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="xs:simpleType" >
      <xsl:for-each select="xs:simpleType" >
        <!-- Simple type in-line -->
        <xsl:call-template name="element_simple_type" >
          <xsl:with-param name="schemaName" select="$schemaName" />
          <xsl:with-param name="maxOccurs" select="$nextMaxOccurs" />
          <xsl:with-param name="minOccurs" select="$nextMinOccurs" />
          <xsl:with-param name="elementElem" select="$elementElem" />
          <xsl:with-param name="choiceName" select="$choiceName" />
				</xsl:call-template>
			</xsl:for-each>			
		</xsl:when>
		<xsl:when test="xs:complexType" >
			<xsl:for-each select="xs:complexType" >
				<!-- Complex type in-line -->
				<xsl:call-template name="element_complex_type" >
					<xsl:with-param name="schemaName" select="$schemaName" />
					<xsl:with-param name="maxOccurs" select="$nextMaxOccurs" />
					<xsl:with-param name="minOccurs" select="$nextMinOccurs" />
					<xsl:with-param name="elementElem" select="$elementElem" />
          <xsl:with-param name="choiceName" select="$choiceName" />
				</xsl:call-template>
			</xsl:for-each>			
		</xsl:when>
	</xsl:choose>
</xsl:template>

<!-- Handles FDO Property based on complex type -->
<xsl:template name="element_type">
	<xsl:param name="schemaName" />
	<xsl:param name="minOccurs" />
	<xsl:param name="maxOccurs" />
	<xsl:param name="elementElem" />
	<xsl:param name="elemTypeUri" />
	<xsl:param name="elemTypeName" />
	<xsl:param name="typeUri" />
	<xsl:param name="typeName" />
  <xsl:param name="choiceName" />
	<xsl:param name="prevTypes" select="/.."/>
	<xsl:param name="restrUri" select="''"/>
	<xsl:param name="restrType" select="''"/>
	<!-- Element has a type. Find the type. First convert qualified
	 type name to URI and local name so we can find it.
	-->
	<xsl:variable name="fdoType">
		<xsl:call-template name="get_fdo_type" >
			<xsl:with-param name="uriName" select="$typeUri" />
			<xsl:with-param name="typeName" select="$typeName" />
		</xsl:call-template>
	</xsl:variable>

	<xsl:variable name="isClass">
		<xsl:call-template name="fdo_type_is_class" >
			<xsl:with-param name="fdoType" select="$fdoType" />
		</xsl:call-template>
	</xsl:variable>

	<xsl:choose>
	    <xsl:when test="$maxOccurs = '0'"/>
		<xsl:when test="$maxOccurs = '1' and $fdoType = 'rasterProperty'">
			<!-- Type is a raster property. -->
            <xsl:call-template name="ElementMapping">
                <xsl:with-param name="elementElem" select="$elementElem"/>
                <xsl:with-param name="typeUri" select="$elemTypeUri"/>
                <xsl:with-param name="typeName" select="$elemTypeName"/>
                <xsl:with-param name="choiceName" select="$choiceName"/>
            </xsl:call-template>

			<xsl:call-template name="RasterProperty" >
				<xsl:with-param name="schemaName" select="$schemaName" />
				<xsl:with-param name="minOccurs" select="$minOccurs" />
				<xsl:with-param name="elementElem" select="$elementElem" />
			</xsl:call-template>
		</xsl:when>
		<xsl:when test="$fdoType = 'geometricProperty'">
			<!-- Type is a GML geometry. -->
			<!-- Generate the geometric property -->
            <xsl:call-template name="ElementMapping">
                <xsl:with-param name="elementElem" select="$elementElem"/>
                <xsl:with-param name="typeUri" select="$elemTypeUri"/>
                <xsl:with-param name="typeName" select="$elemTypeName"/>
                <xsl:with-param name="choiceName" select="$choiceName"/>
            </xsl:call-template>

			<xsl:call-template name="GeometricProperty" >
				<xsl:with-param name="schemaName" select="$schemaName" />
				<xsl:with-param name="elementElem" select="$elementElem" />
                <xsl:with-param name="refUri" select="$elemTypeUri" />
                <xsl:with-param name="refType" select="$elemTypeName" />
                <xsl:with-param name="baseType" select="$typeName" />
				<xsl:with-param name="prevTypes" select="$prevTypes" />
			</xsl:call-template>
		</xsl:when>
		<xsl:when test="$maxOccurs = '1' and $fdoType = 'geometricAssociation'">
			<!-- Type is a GML association to geometry property -->
			<!-- The following handles the association sub-properties,
			      including the geometric property
			-->
            <xsl:call-template name="ElementMapping">
                <xsl:with-param name="elementElem" select="$elementElem"/>
                <xsl:with-param name="typeUri" select="$elemTypeUri"/>
                <xsl:with-param name="typeName" select="$elemTypeName"/>
                <xsl:with-param name="choiceName" select="$choiceName"/>
            </xsl:call-template>

			<xsl:call-template name="GeometricAssociation" >
				<xsl:with-param name="schemaName" select="$schemaName" />
				<xsl:with-param name="elementElem" select="$elementElem" />
                <xsl:with-param name="refUri" select="$elemTypeUri" />
                <xsl:with-param name="refType" select="$elemTypeName" />
                <xsl:with-param name="baseUri" select="$typeUri" />
                <xsl:with-param name="baseType" select="$typeName" />
				<xsl:with-param name="prevTypes" select="$prevTypes" />
                <xsl:with-param name="restrUri" select="$restrUri"/>
                <xsl:with-param name="restrType" select="$restrType"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:when test="$maxOccurs = '1' and $fdoType = 'associationProperty'">
			<!-- Generate the association property -->
            <xsl:call-template name="ElementMapping">
                <xsl:with-param name="elementElem" select="$elementElem"/>
                <xsl:with-param name="typeUri" select="$elemTypeUri"/>
                <xsl:with-param name="typeName" select="$elemTypeName"/>
                <xsl:with-param name="choiceName" select="$choiceName"/>
            </xsl:call-template>

			<xsl:call-template name="AssociationProperty" >
				<xsl:with-param name="schemaName" select="$schemaName" />
				<xsl:with-param name="elementElem" select="$elementElem" />
                <xsl:with-param name="baseUri" select="$typeUri" />
                <xsl:with-param name="baseType" select="$typeName" />
                <xsl:with-param name="restrUri" select="$restrUri"/>
                <xsl:with-param name="restrType" select="$restrType"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:when test="$maxOccurs = '1' and not($fdoType='') and ($isClass='no')">
			<!-- data type is basic type, and this is not a collection.
			 Therefore, it is a data property.
			-->
            <xsl:call-template name="ElementMapping">
                <xsl:with-param name="elementElem" select="$elementElem"/>
                <xsl:with-param name="typeUri" select="$typeUri"/>
                <xsl:with-param name="typeName" select="$typeName"/>
                <xsl:with-param name="choiceName" select="$choiceName"/>
            </xsl:call-template>

			<xsl:call-template name="DataProperty" >
				<xsl:with-param name="schemaName" select="$schemaName" />
				<xsl:with-param name="minOccurs" select="$minOccurs" />
				<xsl:with-param name="elementElem" select="$elementElem" />
				<xsl:with-param name="dataType" select="$fdoType" />
				<xsl:with-param name="prevTypes" select="$prevTypes" />
			</xsl:call-template>
		</xsl:when>
	    <xsl:when test="not($maxOccurs = '1')">
	        <!-- It is a collection so treat it as an object property -->
            <xsl:call-template name="ElementMapping">
                <xsl:with-param name="elementElem" select="$elementElem"/>
                <xsl:with-param name="typeUri" select="$elemTypeUri"/>
                <xsl:with-param name="typeName" select="$elemTypeName"/>
                <xsl:with-param name="choiceName" select="$choiceName"/>
            </xsl:call-template>

			<xsl:call-template name="ObjectProperty" >
			    <xsl:with-param name="schemaName" select="$schemaName" />
			    <xsl:with-param name="maxOccurs" select="$maxOccurs" />
			    <xsl:with-param name="elementElem" select="$elementElem"/>
			    <xsl:with-param name="refUri" select="$elemTypeUri" />
			    <xsl:with-param name="refType" select="$elemTypeName" />
		   </xsl:call-template>
    	</xsl:when>
		<!-- Otherwise find the type in current document -->
		<xsl:otherwise>
		    <xsl:variable name="gblType" select="key('gblTypeKey',concat($typeUri,':',$typeName))"/>
		    <xsl:choose>
    		    <xsl:when test="count($gblType) = 0">
			        <!-- It is a type not defined in current document
			        treat it as an object property
			        -->
					<xsl:call-template name="ElementMapping">
						<xsl:with-param name="elementElem" select="$elementElem"/>
						<xsl:with-param name="typeUri" select="$elemTypeUri"/>
						<xsl:with-param name="typeName" select="$elemTypeName"/>
            <xsl:with-param name="choiceName" select="$choiceName"/>
					</xsl:call-template>

			        <xsl:call-template name="ObjectProperty" >
				        <xsl:with-param name="schemaName" select="$schemaName" />
				        <xsl:with-param name="maxOccurs" select="$maxOccurs" />
				        <xsl:with-param name="elementElem" select="$elementElem"/>
				        <xsl:with-param name="refUri" select="$elemTypeUri" />
				        <xsl:with-param name="refType" select="$elemTypeName" />
			        </xsl:call-template>
    		    </xsl:when>
    		    <xsl:otherwise>
    		        <xsl:for-each select="$gblType[position()=1]">
    		            <xsl:variable name="nextTypes" select="$prevTypes|."/>
    		            <xsl:variable name="baseType">
    		                <xsl:choose>
    		                    <xsl:when test="xs:restriction">
    		                        <xsl:value-of select="xs:restriction/@base"/>
    		                    </xsl:when>
    		                    <xsl:when test="xs:complexContent/xs:restriction">
    		                        <xsl:value-of select="xs:complexContent/xs:restriction/@base"/>
    		                    </xsl:when>
    		                    <xsl:when test="xs:complexContent/xs:extension">
    		                        <xsl:value-of select="xs:complexContent/xs:extension/@base"/>
    		                    </xsl:when>
    		                </xsl:choose>
    		            </xsl:variable>
    		            <xsl:choose>
    		                <xsl:when test="$baseType=''">
    		                    <!-- Type has no base and is not a basic, geometry or 
    		                         association type. It must be Class Definition, so 
    		                         create an Object Property that references it.
    		                    -->
								<xsl:call-template name="ElementMapping">
									<xsl:with-param name="elementElem" select="$elementElem"/>
									<xsl:with-param name="typeUri" select="$elemTypeUri"/>
									<xsl:with-param name="typeName" select="$elemTypeName"/>
                  <xsl:with-param name="choiceName" select="$choiceName"/>
								</xsl:call-template>

			                    <xsl:call-template name="ObjectProperty" >
				                    <xsl:with-param name="schemaName" select="$schemaName" />
				                    <xsl:with-param name="maxOccurs" select="$maxOccurs" />
				                    <xsl:with-param name="elementElem" select="$elementElem"/>
				                    <xsl:with-param name="refUri" select="$elemTypeUri" />
				                    <xsl:with-param name="refType" select="$elemTypeName" />
			                    </xsl:call-template>
    		                </xsl:when>
    		                <xsl:otherwise>
            		            <xsl:variable name="nextRestr">
            		                <xsl:choose>
            		                    <xsl:when test="$restrUri = ''">
            		                        <xsl:choose>
    	        	                            <xsl:when test="xs:restriction|xs:complexContent/xs:restriction">
    		                                        <xsl:value-of select="'yes'"/>
            		                            </xsl:when>
    	        	                            <xsl:otherwise>
    		                                        <xsl:value-of select="'no'"/>
            		                            </xsl:otherwise>
    	        	                        </xsl:choose>
    		                            </xsl:when>
    		                            <xsl:otherwise>
    		                                <xsl:value-of select="'no'"/>
    		                            </xsl:otherwise>
    		                        </xsl:choose>
    		                    </xsl:variable>
            		            <xsl:variable name="nextRestrUri">
            		                <xsl:choose>
            		                    <xsl:when test="$nextRestr = 'yes'">
	                                        <xsl:value-of select="$typeUri"/>
            		                    </xsl:when>
    	        	                    <xsl:otherwise>
	                                        <xsl:value-of select="$restrUri"/>
       		                            </xsl:otherwise>
    		                        </xsl:choose>
    		                    </xsl:variable>
            		            <xsl:variable name="nextRestrType">
            		                <xsl:choose>
            		                    <xsl:when test="$nextRestr = 'yes'">
	                                        <xsl:value-of select="$typeName"/>
            		                    </xsl:when>
    	        	                    <xsl:otherwise>
	                                        <xsl:value-of select="$restrType"/>
       		                            </xsl:otherwise>
    		                        </xsl:choose>
    		                    </xsl:variable>
    		                    <!-- Step down to the current type's base type -->
	                            <xsl:variable name="fullBase">
		                            <xsl:call-template name="get_fullname" >
			                            <xsl:with-param name="typeName" select="$baseType" />
		                            </xsl:call-template>			
	                            </xsl:variable>
	                            <xsl:variable name="baseUri"><xsl:value-of select="substring-before($fullBase,' ')"/></xsl:variable>
	                            <xsl:variable name="baseName"><xsl:value-of select="substring-before(substring-after($fullBase,' '),' ')"/></xsl:variable>
				                <xsl:call-template name="element_type" >
					                <xsl:with-param name="schemaName" select="$schemaName" />
					                <xsl:with-param name="maxOccurs" select="$maxOccurs" />
					                <xsl:with-param name="minOccurs" select="$minOccurs" />
					                <xsl:with-param name="elementElem" select="$elementElem" />
                          <xsl:with-param name="choiceName" select="$choiceName" />
	                                <xsl:with-param name="elemTypeUri" select="$elemTypeUri"/>
	                                <xsl:with-param name="elemTypeName" select="$elemTypeName"/>
	                                <xsl:with-param name="typeUri" select="$baseUri"/>
	                                <xsl:with-param name="typeName" select="$baseName"/>
	                                <xsl:with-param name="prevTypes" select="$nextTypes"/>
				                    <xsl:with-param name="restrUri" select="$nextRestrUri"/>
				                    <xsl:with-param name="restrType" select="$nextRestrType"/>
				                </xsl:call-template>
    		                </xsl:otherwise>
    		            </xsl:choose>
    		        </xsl:for-each>
    		    </xsl:otherwise>
		    </xsl:choose>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- Handles FDO Property based on simple type -->
<xsl:template name="element_simple_type">
	<xsl:param name="schemaName" />
	<xsl:param name="minOccurs" />
	<xsl:param name="maxOccurs" />
	<xsl:param name="elementElem" />
  <xsl:param name="choiceName" />
	<xsl:param name="prevTypes" select="."/>

    <xsl:variable name="fullType">
        <xsl:call-template name="get_fullname" >
	        <xsl:with-param name="typeName" select="xs:restriction/@base" />
        </xsl:call-template>			
    </xsl:variable>
    <xsl:variable name="typeUri"><xsl:value-of select="substring-before($fullType,' ')"/></xsl:variable>
    <xsl:variable name="typeName"><xsl:value-of select="substring-before(substring-after($fullType,' '),' ')"/></xsl:variable>
    <xsl:call-template name="element_type" >
	  <xsl:with-param name="schemaName" select="$schemaName" />
		<xsl:with-param name="minOccurs" select="$minOccurs" />
		<xsl:with-param name="maxOccurs" select="$maxOccurs" />
		<xsl:with-param name="elementElem" select="$elementElem" />
		<xsl:with-param name="elemTypeUri" select="$typeUri" />
		<xsl:with-param name="elemTypeName" select="$typeName" />
		<xsl:with-param name="typeUri" select="$typeUri" />
		<xsl:with-param name="typeName" select="$typeName" />
		<xsl:with-param name="prevTypes" select="$prevTypes" />
    <xsl:with-param name="choiceName" select="$choiceName" />
	</xsl:call-template>
	<xsl:call-template name="simpleType_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
</xsl:template>

  <!-- Handles FDO Property based on complex type -->
  <xsl:template name="element_complex_type">
    <xsl:param name="schemaName" />
    <xsl:param name="minOccurs" />
    <xsl:param name="maxOccurs" />
    <xsl:param name="elementElem" />
    <xsl:param name="choiceName" />
    <xsl:param name="prevTypes" select="."/>

    <!-- Currently only restrictions handled. Other cases require that an FDO class is generated for this inline complex type. -->
    <xsl:choose>
      <xsl:when test="xs:complexContent/xs:restriction/@base">
        <xsl:variable name="fullType">
          <xsl:call-template name="get_fullname" >
            <xsl:with-param name="typeName" select="xs:complexContent/xs:restriction/@base" />
          </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="typeUri">
          <xsl:value-of select="substring-before($fullType,' ')"/>
        </xsl:variable>
        <xsl:variable name="typeName">
          <xsl:value-of select="substring-before(substring-after($fullType,' '),' ')"/>
        </xsl:variable>
        <xsl:call-template name="element_type" >
          <xsl:with-param name="schemaName" select="$schemaName" />
          <xsl:with-param name="minOccurs" select="$minOccurs" />
          <xsl:with-param name="maxOccurs" select="$maxOccurs" />
          <xsl:with-param name="elementElem" select="$elementElem" />
          <xsl:with-param name="elemTypeUri" select="$typeUri" />
          <xsl:with-param name="elemTypeName" select="$typeName" />
          <xsl:with-param name="typeUri" select="$typeUri" />
          <xsl:with-param name="typeName" select="$typeName" />
          <xsl:with-param name="prevTypes" select="$prevTypes" />
          <xsl:with-param name="choiceName" select="$choiceName" />
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="xs:sequence/xs:element/@ref" >
        <!-- This element references another element, follow the reference -->
        <!-- convert referenced element's qualified name to URI plus local name -->
        <xsl:variable name="fullName">
          <xsl:call-template name="get_fullname" >
            <xsl:with-param name="typeName" select="xs:sequence/xs:element/@ref" />
          </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="uriName">
          <xsl:value-of select="substring-before($fullName,' ')"/>
        </xsl:variable>
        <xsl:variable name="localName">
          <xsl:value-of select="substring-before(substring-after($fullName,' '),' ')"/>
        </xsl:variable>
        <xsl:variable name="elemType">
          <xsl:call-template name="get_element_fdo_type" >
            <xsl:with-param name="uriName" select="$uriName" />
            <xsl:with-param name="elemName" select="$localName" />
          </xsl:call-template>
        </xsl:variable>

        <xsl:variable name="fdoType">
          <xsl:call-template name="get_fdo_type" >
            <xsl:with-param name="uriName" select="$uriName" />
            <xsl:with-param name="typeName" select="$elemType" />
          </xsl:call-template>
        </xsl:variable>

        <xsl:if test="$fdoType = 'geometricProperty' or ($maxOccurs = '1' and $fdoType = 'geometricAssociation')">
          <!-- URI and local name indicate that this is a Geometric property -->
          <xsl:call-template name="ElementMapping">
            <xsl:with-param name="elementElem" select="$elementElem"/>
            <xsl:with-param name="typeUri" select="$uriName"/>
            <xsl:with-param name="typeName" select="$elemType"/>
            <xsl:with-param name="choiceName" select="$choiceName"/>
          </xsl:call-template>

          <xsl:call-template name="GeometricProperty" >
            <xsl:with-param name="schemaName" select="$schemaName" />
            <xsl:with-param name="elementElem" select="$elementElem"/>
            <xsl:with-param name="refUri" select="$uriName"/>
            <xsl:with-param name="refType" select="$localName"/>
            <xsl:with-param name="baseType" select="$localName"/>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <!-- Writes out the GML-FDO mappings for an element -->
<xsl:template name="ElementMapping">
    <!-- the element -->
	<xsl:param name="elementElem" />
	<!-- schema for element's type -->
	<xsl:param name="typeUri" />
	<!--element's type -->
	<xsl:param name="typeName" />
  <!--is choice element -->
  <xsl:param name="choiceName" />

  <xsl:variable name="elemName">
    <xsl:choose>
      <xsl:when test="$elementElem/@name">
      	<xsl:value-of select="$elementElem/@name" />
      </xsl:when>
      <xsl:otherwise>
    		<xsl:value-of select="$elementElem/@ref" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
    
  <xsl:element name="ElementMapping">
    <xsl:attribute name="name" >
      <xsl:value-of select="translate($elemName,':','/')"/>
    </xsl:attribute>

    <xsl:choose>      
      <xsl:when test="$elementElem/@ref">
        <xsl:variable name="elemFullName">
          <xsl:call-template name="get_fullname" >
            <xsl:with-param name="typeName" select="$elemName" />
          </xsl:call-template>
        </xsl:variable>

        <xsl:variable name="elemUri">
          <xsl:value-of select="substring-before($elemFullName,' ')"/>
        </xsl:variable>
        <xsl:variable name="elemLocalName">
          <xsl:value-of select="substring-before(substring-after($elemFullName,' '),' ')"/>
        </xsl:variable>
        <xsl:attribute name="gmlUri">
          <xsl:value-of select="$elemUri"/>
        </xsl:attribute>
        <xsl:attribute name="gmlLocalName">
          <xsl:value-of select="$elemLocalName"/>
        </xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="gmlLocalName">
          <xsl:value-of select="$elemName"/>
        </xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:attribute name="classSchema">
      <xsl:call-template name="uri_2_schema_name">
        <xsl:with-param name="uri" select="$typeUri" />
      </xsl:call-template>
    </xsl:attribute>
    <xsl:attribute name="className">
      <xsl:call-template name="type_to_class">
        <xsl:with-param name="type" select="$typeName" />
      </xsl:call-template>
    </xsl:attribute>
    <xsl:if test="not($choiceName='')">
      <xsl:attribute name="choiceName">
        <xsl:value-of select="$choiceName"/>
      </xsl:attribute>
    </xsl:if>
  </xsl:element>
</xsl:template>

<!-- Generates an FDO Data Property -->
<xsl:template name="DataProperty">
	<xsl:param name="schemaName" />
	<xsl:param name="minOccurs" />
	<xsl:param name="elementElem" />
	<xsl:param name="dataType" />
	<xsl:param name="prevTypes" select="/.." />
	<xsl:element name="DataProperty">
	    <xsl:call-template name="property_attributes">
	        <xsl:with-param name="schemaName" select="$schemaName"/>
	        <xsl:with-param name="elementElem" select="$elementElem"/>
	    </xsl:call-template>
		<xsl:attribute name="dataType">
			<xsl:value-of select="$dataType" />
		</xsl:attribute>
		<!-- Pick out various data property facets from the 
		     property's type and ancestor types. When a facet appears
		     in more than one type, the most restrictive value is 
		     chosen.
		-->
	    <xsl:variable name="length">
	        <xsl:call-template name="get_minmax_facet">
	            <xsl:with-param name="types" select="$prevTypes"/>
	            <xsl:with-param name="facetName" select="'maxLength'"/>
	            <xsl:with-param name="min" select="y"/>
	        </xsl:call-template>
	    </xsl:variable>
	    <xsl:variable name="precision">
	        <xsl:call-template name="get_minmax_facet">
	            <xsl:with-param name="types" select="$prevTypes"/>
	            <xsl:with-param name="facetName" select="'totalDigits'"/>
	            <xsl:with-param name="min" select="y"/>
	        </xsl:call-template>
	    </xsl:variable>
	    <xsl:variable name="scale">
	        <xsl:choose>
                <xsl:when test="xs:restriction/xs:fractionDigits/@fdo:scale">
                    <!-- fdo:scale overrides the scale value. This
                        can happen when scale is negative.
                    -->
    		        <xsl:value-of select="xs:restriction/xs:fractionDigits/@fdo:scale" />
	            </xsl:when>
                <xsl:when test="xs:restriction/xs:fractionDigits/@fdov2:scale">
    		        <xsl:value-of select="xs:restriction/xs:fractionDigits/@fdov2:scale" />
	            </xsl:when>
	            <xsl:otherwise>
	                <xsl:call-template name="get_minmax_facet">
	                    <xsl:with-param name="types" select="$prevTypes"/>
	                    <xsl:with-param name="facetName" select="'fractionDigits'"/>
        	            <xsl:with-param name="min" select="y"/>
	                </xsl:call-template>
	            </xsl:otherwise>
	       </xsl:choose>
	    </xsl:variable>
	    <xsl:variable name="minExclusive">
	        <xsl:call-template name="get_minmax_facet">
	            <xsl:with-param name="types" select="$prevTypes"/>
	            <xsl:with-param name="facetName" select="'minExclusive'"/>
	            <xsl:with-param name="min" select="n"/>
	        </xsl:call-template>
	    </xsl:variable>
	    <xsl:variable name="minInclusive">
	        <xsl:call-template name="get_minmax_facet">
	            <xsl:with-param name="types" select="$prevTypes"/>
	            <xsl:with-param name="facetName" select="'minInclusive'"/>
	            <xsl:with-param name="min" select="n"/>
	        </xsl:call-template>
	    </xsl:variable>
	    <xsl:variable name="maxExclusive">
	        <xsl:call-template name="get_minmax_facet">
	            <xsl:with-param name="types" select="$prevTypes"/>
	            <xsl:with-param name="facetName" select="'maxExclusive'"/>
	            <xsl:with-param name="min" select="y"/>
	        </xsl:call-template>
	    </xsl:variable>
	    <xsl:variable name="maxInclusive">
	        <xsl:call-template name="get_minmax_facet">
	            <xsl:with-param name="types" select="$prevTypes"/>
	            <xsl:with-param name="facetName" select="'maxInclusive'"/>
	            <xsl:with-param name="min" select="y"/>
	        </xsl:call-template>
	    </xsl:variable>

    <xsl:variable name="enums">
      <xsl:call-template name="get_enum_facets">
        <xsl:with-param name="types" select="$prevTypes"/>
      </xsl:call-template>
    </xsl:variable>
    
		<!-- Write out the string type facets as attributes -->
	    <xsl:if test="not($length='') and ($dataType='string')">
	        <xsl:attribute name="length">
	            <xsl:value-of select="$length"/>
	        </xsl:attribute>
	    </xsl:if>

		<!-- Write out the decimal property type facets as attributes -->
	    <xsl:if test="$dataType='decimal'" >
		    <xsl:if test="not($precision='')">
			    <xsl:attribute name="precision">
				    <xsl:value-of select="$precision" />
			    </xsl:attribute>
		    </xsl:if>
		    <xsl:if test="not($scale='')">
			    <xsl:attribute name="scale">
				    <xsl:value-of select="$scale" />
			    </xsl:attribute>
		    </xsl:if>
	    </xsl:if>
	    
	    <!-- Set readonly, nullability, autogeneration setting and default value -->
		<xsl:if test="$elementElem/@fdo:readOnly = 'true'" >
			<xsl:attribute name="readOnly" >true</xsl:attribute>
		</xsl:if>
		<xsl:if test="$elementElem/@fdov2:readOnly = 'true'" >
			<xsl:attribute name="readOnly" >true</xsl:attribute>
		</xsl:if>
		<xsl:choose>
			<xsl:when test="$minOccurs='0'" >
				<xsl:attribute name="nullable" >true</xsl:attribute>
			</xsl:when>
			<xsl:otherwise>
				<xsl:attribute name="nullable" >false</xsl:attribute>
			</xsl:otherwise>
		</xsl:choose>
		<xsl:if test="$elementElem/@default" >
			<xsl:attribute name="default" >
				<xsl:value-of select="$elementElem/@default" />
			</xsl:attribute>
		</xsl:if>
		<xsl:if test="$elementElem/@fdo:autogenerated = 'true'" >
			<xsl:attribute name="autogenerated" >true</xsl:attribute>
		</xsl:if>
		<xsl:if test="$elementElem/@fdov2:autogenerated = 'true'" >
			<xsl:attribute name="autogenerated" >true</xsl:attribute>
		</xsl:if>
		
		<!-- length facet valid only for strings -->
	    <xsl:if test="not($length='') and not($dataType='string')" >
            <xsl:call-template name="unsupported_restriction_type">
                <xsl:with-param name="schemaName" select="$schemaName"/>
                <xsl:with-param name="facetName">maxLength</xsl:with-param>
            </xsl:call-template>
        </xsl:if>
		
		<!-- totalDigits and fractionDigits facet valid only for decimal numbers -->
	    <xsl:if test="not($dataType='decimal')" >
	        <xsl:if test="not($precision='')">
	            <xsl:call-template name="unsupported_restriction_type">
	                <xsl:with-param name="schemaName" select="$schemaName"/>
	                <xsl:with-param name="facetName">totalDigits</xsl:with-param>
	            </xsl:call-template>
	        </xsl:if>
	        <xsl:if test="not($scale='')">
	            <xsl:call-template name="unsupported_restriction_type">
	                <xsl:with-param name="schemaName" select="$schemaName"/>
	                <xsl:with-param name="facetName">fractionDigits</xsl:with-param>
	            </xsl:call-template>
	        </xsl:if>
        </xsl:if>
        
		<!-- Write out general sub-elements -->
		<xsl:for-each select="$elementElem">
			<xsl:call-template name="element_subelements" />
		</xsl:for-each>
		
		<!-- Write out any enumeration facets as sub-elements -->
		<!--<xsl:if test="not($enums='')">
		    <xsl:for-each select="$enums">
			    <xsl:element name="enumeration">
			        <xsl:value-of select="."/>
			    </xsl:element>
		    </xsl:for-each>
		</xsl:if>-->

    <xsl:variable name="listCount" select="count($prevTypes/xs:restriction/xs:enumeration)"/>
    <xsl:if test="$listCount &gt; 0">
      <xsl:element name="Constraint">
        <xsl:attribute name="type">list</xsl:attribute>
        <xsl:element name="Values">
          <xsl:for-each select="$prevTypes/xs:restriction/xs:enumeration">
            <xsl:element name="Value">
              <xsl:value-of select="./@value"/>
            </xsl:element>
          </xsl:for-each>
        </xsl:element>
      </xsl:element>
    </xsl:if>
    
    <xsl:if test="not($minExclusive='') or not($minInclusive='') or not($maxExclusive='') or not($maxInclusive='')">
      <xsl:element name="Constraint">
        <xsl:attribute name="type">range</xsl:attribute>
        
        <!-- Write the facets that apply to almost all types -->
        <xsl:if test="not($minExclusive='')">
          <xsl:element name="Min">
            <xsl:attribute name="inclusive">false</xsl:attribute>
            <xsl:attribute name="value">
              <xsl:value-of select="$minExclusive" />
            </xsl:attribute>
          </xsl:element>
        </xsl:if>

        <xsl:if test="not($minInclusive='')">
          <xsl:element name="Min">
            <xsl:attribute name="inclusive">true</xsl:attribute>
            <xsl:attribute name="value">
              <xsl:value-of select="$minInclusive" />
            </xsl:attribute>
          </xsl:element>
        </xsl:if>

        <xsl:if test="not($maxExclusive='')">
          <xsl:element name="Max">
            <xsl:attribute name="inclusive">false</xsl:attribute>
            <xsl:attribute name="value">
              <xsl:value-of select="$maxExclusive" />
            </xsl:attribute>
          </xsl:element>
        </xsl:if>

        <xsl:if test="not($maxInclusive='')">
          <xsl:element name="Max">
            <xsl:attribute name="inclusive">true</xsl:attribute>
            <xsl:attribute name="value">
              <xsl:value-of select="$maxInclusive" />
            </xsl:attribute>
          </xsl:element>
        </xsl:if>
      </xsl:element>
    </xsl:if>
    
	</xsl:element>
</xsl:template>

<!-- Generates an FDO Object Property -->
<xsl:template name="ObjectProperty">
	<xsl:param name="schemaName" />
	<xsl:param name="maxOccurs" />
	<xsl:param name="elementElem" />
	<xsl:param name="refUri" />
	<xsl:param name="refType" />
	<xsl:element name="ObjectProperty">
	    <xsl:call-template name="property_attributes">
	        <xsl:with-param name="schemaName" select="$schemaName"/>
	        <xsl:with-param name="elementElem" select="$elementElem"/>
	    </xsl:call-template>
		<xsl:attribute name="classSchema">
		    <xsl:for-each select="$elementElem">
		        <xsl:call-template name="uri_2_schema_name">
    			    <xsl:with-param name="uri" select="$refUri" />
		        </xsl:call-template>
		    </xsl:for-each>
		</xsl:attribute>
		<xsl:attribute name="class">
            <xsl:call-template name="type_to_class">
                <xsl:with-param name="type" select="$refType" />		                    
            </xsl:call-template>
		</xsl:attribute>
		<xsl:choose>
			<!-- non-collection is value property -->
			<xsl:when test="$maxOccurs='1'">
				<xsl:attribute name="objectType">value</xsl:attribute>
			</xsl:when>
			<xsl:otherwise>
				<xsl:if test="$elementElem/@fdo:identityProperty">
					<xsl:attribute name="identityProperty">
						<xsl:value-of select="$elementElem/@fdo:identityProperty" />
					</xsl:attribute>
				</xsl:if>
				<xsl:if test="$elementElem/@fdov2:identityProperty">
					<xsl:attribute name="identityProperty">
						<xsl:value-of select="$elementElem/@fdov2:identityProperty" />
					</xsl:attribute>
				</xsl:if>
				<xsl:choose>
					<xsl:when test="$elementElem/@fdo:order">
						<xsl:attribute name="objectType">orderedcollection</xsl:attribute>
						<xsl:attribute name="orderType">
							<xsl:value-of select="$elementElem/@fdo:order" />
						</xsl:attribute>
					</xsl:when>
					<xsl:when test="$elementElem/@fdov2:order">
						<xsl:attribute name="objectType">orderedcollection</xsl:attribute>
						<xsl:attribute name="orderType">
							<xsl:value-of select="$elementElem/@fdov2:order" />
						</xsl:attribute>
					</xsl:when>
					<xsl:otherwise>
						<xsl:attribute name="objectType">collection</xsl:attribute>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
		<xsl:for-each select="$elementElem">
			<xsl:call-template name="element_subelements" />
		</xsl:for-each>
	</xsl:element>
</xsl:template>

<!-- Generates an FDO Association Property -->
<xsl:template name="AssociationProperty">
	<xsl:param name="schemaName" />
	<xsl:param name="elementElem" select="."/>
    <xsl:param name="baseUri"/>
    <xsl:param name="baseType"/>
    <xsl:param name="restrUri" select="''"/>
    <xsl:param name="restrType" select="''"/>
    
	<xsl:element name="AssociationProperty">
	    <!--Transform the attributes -->
	    <xsl:call-template name="property_attributes">
	        <xsl:with-param name="schemaName" select="$schemaName"/>
	        <xsl:with-param name="elementElem" select="$elementElem"/>
	    </xsl:call-template>
		<xsl:variable name="defaultIdentity">
			<xsl:choose>
				<xsl:when test="@fdov2:defaultIdentity">
					<xsl:value-of select="@fdov2:defaultIdentity"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="@fdo:defaultIdentity"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>

		<xsl:call-template name="association_attributes">
	        <xsl:with-param name="schemaName" select="$schemaName"/>
	        <xsl:with-param name="elementElem" select="$elementElem"/>
		</xsl:call-template>
		
		<!-- Transform the associated class. Depends on whether association is by 
		     FeatureProperty or keyref.
		-->
        <xsl:choose>
            <xsl:when test="xs:keyref">
		        <!-- Association is by keyref. To get to the associated class we must find
		             the element containing the key pointed to by this element's keyref. The associated
		             class is the containing element's type 
		        -->
		        <!-- First, get the full name of the referenced key -->
    	        <xsl:variable name="fullAssocElem">
			        <xsl:call-template name="get_fullname" >
				        <xsl:with-param name="typeName" select="xs:keyref/@refer"/>
			        </xsl:call-template>
		        </xsl:variable>
		        <!-- from full name, generated the name qualified by uri -->
    	        <xsl:variable name="qualAssocElem" select="concat(substring-before($fullAssocElem,' '),':',substring-before(substring-after($fullAssocElem,' '),' '))"/>
	            <!-- Find the reference key, can be an xs:key or xs:unique key. -->
	            <xsl:variable name="assocByKey" select="key('keyKey',$qualAssocElem)" />
		        <xsl:variable name="assocByUnique" select="key('uniqueKey',$qualAssocElem)" />
		        <!-- Determine the full name of the associated class from the referenced key
		        TODO (future): the associated class is assumed to be the type of the element
		        containing the referenced key. The proper way to find this element would be 
		        to follow the referenced key's selector. However, until we support importing
		        from external sources, the element referenced by the selector is always the 
		        containing element.  -->
    	        <xsl:variable name="fullAssocType">
			        <xsl:call-template name="get_fullname" >
				        <xsl:with-param name="typeName">
				            <xsl:choose>
				                <xsl:when test="$assocByKey">
				                    <xsl:value-of select="$assocByKey/../@type" />
				                </xsl:when>
				                <xsl:when test="$assocByUnique">
				                    <xsl:value-of select="$assocByUnique/../@type" />
				                </xsl:when>
				            </xsl:choose>
				        </xsl:with-param>
			        </xsl:call-template>			
		        </xsl:variable>

                <!-- Parse out the uri, local name and namespace from the full associated class name -->
		        <xsl:variable name="uriName"><xsl:value-of select="substring-before($fullAssocType,' ')"/></xsl:variable>
		        <xsl:variable name="localName"><xsl:value-of select="substring-before(substring-after($fullAssocType,' '),' ')"/></xsl:variable>
		        <xsl:variable name="nameSpace"><xsl:value-of select="substring-after(substring-after($fullAssocType,' '),' ')"/></xsl:variable>

                <!-- Schema is the name space -->
		        <xsl:attribute name="associatedClassSchema">
		            <xsl:for-each select="$elementElem">
		                <xsl:call-template name="uri_2_schema_name">
		                    <xsl:with-param name="uri" select="$uriName"/>
		                </xsl:call-template>
		            </xsl:for-each>
		        </xsl:attribute>
		        <!-- Class name is the local name -->
		        <xsl:attribute name="associatedClass">
                    <xsl:call-template name="type_to_class">
                        <xsl:with-param name="type" select="$localName" />		                    
                    </xsl:call-template>
		        </xsl:attribute>
        		
		        <!-- Determine of this is a default type of association (identity properties are
		        implicitly the identity properties of the associated class
		        -->
		        <xsl:variable name="defaultAssoc" >
		            <xsl:choose>
	                    <xsl:when test="$assocByKey and not($defaultIdentity='false')">
	                        <xsl:value-of select="'true'" />
	                    </xsl:when>
	                    <xsl:otherwise>
	                        <xsl:value-of select="'false'" />
	                    </xsl:otherwise>
		            </xsl:choose>
		        </xsl:variable>
        		
		        <!-- Get the identity properties from the keyref field list 
		        Set the defaultAssoc attribute when identity properties are implicit
		        -->
		        <xsl:element name="IdentityProperties" >
		            <xsl:if test="$defaultAssoc='true'">
		                <xsl:attribute name="default">true</xsl:attribute>
		            </xsl:if>
		            <xsl:for-each select="xs:keyref/xs:field">
		                <xsl:element name="IdentityProperty">
		                    <xsl:value-of select="@xpath"/>
		                </xsl:element>
		            </xsl:for-each>
		        </xsl:element>
        		
		        <!-- Get the reverse identity properties from sub-properties of the association
		        property element
		        -->
		        <xsl:element name="IdentityReverseProperties" >
		            <xsl:if test="$defaultAssoc='true'">
		                <xsl:attribute name="default">true</xsl:attribute>
		            </xsl:if>
			        <xsl:apply-templates select="xs:complexType/xs:sequence" >
				        <xsl:with-param name="schemaName" select="$schemaName" />
				        <xsl:with-param name="minOccurs">1</xsl:with-param>
				        <xsl:with-param name="maxOccurs">1</xsl:with-param>
			        </xsl:apply-templates>
		        </xsl:element>
            </xsl:when>
            <xsl:otherwise>
				<xsl:choose>
					<xsl:when test="xs:complexContent/xs:restriction">
						<!-- Property type is inline. Type is always a restriction, where the first sub-element defines the type for the associated class -->
						<xsl:call-template name="AssociationPropertyGml">
							<xsl:with-param name="elementElem" select="$elementElem"/>
							<xsl:with-param name="assocElem" select="xs:complexContent/xs:restriction/xs:sequence/xs:element[position()=1]"/>
						</xsl:call-template>
					</xsl:when>
					<xsl:otherwise>
						<!-- Property type not inline, lookup the base type -->
						<xsl:variable name="containerUri">
							<xsl:choose>
								<xsl:when test="$restrUri=''">
									<xsl:value-of select="$baseUri"/>
								</xsl:when>
								<xsl:otherwise>
									<xsl:value-of select="$restrUri" />   
								</xsl:otherwise>
							</xsl:choose>
						</xsl:variable>
				
						<xsl:variable name="containerType">
							<xsl:choose>
								<xsl:when test="$restrType=''">
									<xsl:value-of select="$baseType"/>
								</xsl:when>
								<xsl:otherwise>
									<xsl:value-of select="$restrType" />   
								</xsl:otherwise>
							</xsl:choose>
						</xsl:variable>
						<xsl:variable name="containerElem" select="key('gblTypeKey',concat($containerUri,':',$containerType))" />
						<xsl:choose>
							<!-- If base type definition found, it will be a restriction or sequence with first sub-element defining the type for the associated class -->
							<xsl:when test="not(count($containerElem) = 0)">
								<xsl:choose>
									<xsl:when test="$containerElem/xs:complexContent/xs:restriction">
										<xsl:call-template name="AssociationPropertyGml">
											<xsl:with-param name="elementElem" select="$elementElem"/>
											<xsl:with-param name="assocElem" select="$containerElem/xs:complexContent/xs:restriction/xs:sequence/xs:element[position()=1]"/>
										</xsl:call-template>
									</xsl:when>
									<xsl:when test="$containerElem/xs:sequence">
										<xsl:call-template name="AssociationPropertyGml">
											<xsl:with-param name="elementElem" select="$elementElem"/>
											<xsl:with-param name="assocElem" select="$containerElem/xs:sequence/xs:element[position()=1]"/>
										</xsl:call-template>
									</xsl:when>
								</xsl:choose>
							</xsl:when>
							<!-- If not found, it must be a well-known GML association type -->
							<xsl:otherwise>
								<xsl:call-template name="AssociationPropertyGmlDangling">
									<xsl:with-param name="typeUri" select="$containerUri"/>
									<xsl:with-param name="localName" select="$containerType"/>
								</xsl:call-template>
							</xsl:otherwise>
						</xsl:choose>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
		<xsl:for-each select="$elementElem">
			<xsl:call-template name="element_subelements"/>
		</xsl:for-each>
	</xsl:element>
</xsl:template>

<!-- Writes an association property for an element based on a GML style association -->
<xsl:template name="AssociationPropertyGml">
	<xsl:param name="elementElem"/>
	<xsl:param name="assocElem"/>
	
	<!-- convert type of association element (1st sub-element of GML association restriction) to uri and local name -->
    <xsl:if test="$assocElem">
        <xsl:variable name="fullAssocElemType">
			<xsl:call-template name="get_element_type" >
			    <xsl:with-param name="elementElem" select="$assocElem"/>
		    </xsl:call-template>
	    </xsl:variable>
        <!-- Parse out the uri, local name and namespace from the full associated class name -->
	    <xsl:variable name="uriName"><xsl:value-of select="substring-before($fullAssocElemType,' ')"/></xsl:variable>
	    <xsl:variable name="localName"><xsl:value-of select="substring-before(substring-after($fullAssocElemType,' '),' ')"/></xsl:variable>
        <!-- Convert association element uri to associated class schema name -->
	    <xsl:attribute name="associatedClassSchema">
	        <xsl:for-each select="$elementElem">
	            <xsl:call-template name="uri_2_schema_name">
	                <xsl:with-param name="uri" select="$uriName"/>
	            </xsl:call-template>
	        </xsl:for-each>
		</xsl:attribute>
        <!-- Convert association element local name to associated class schema name -->
	    <xsl:attribute name="associatedClass">
            <xsl:call-template name="type_to_class">
                <xsl:with-param name="type" select="$localName" />		                    
            </xsl:call-template>
		</xsl:attribute>
    </xsl:if>
</xsl:template>

<!-- Writes an association property for an element based on a GML style association whose definition is not
      present in the input file
-->
<xsl:template name="AssociationPropertyGmlDangling">
	<xsl:param name="typeUri"/>
	<xsl:param name="localName"/>
	
	<xsl:attribute name="associatedClassSchema">
		<xsl:call-template name="uri_2_schema_name">
			<xsl:with-param name="uri" select="$typeUri"/>
		</xsl:call-template>
	</xsl:attribute>

	<xsl:choose>
		<xsl:when test="$typeUri = $gGmlUri and ($localName = 'FeatureAssociationType' or $localName = 'FeaturePropertyType')">
			<!-- It's a base gml association type so associated to any feature -->
			<xsl:attribute name="associatedClass" >
				<xsl:value-of select="'AbstractFeature'"/>
			</xsl:attribute>
		</xsl:when>
		<xsl:otherwise>
			<!-- This should never happen but if it does, generated associated class from local name passed in. -->
			<xsl:attribute name="associatedClass" >
				<xsl:call-template name="type_to_class">
					<xsl:with-param name="type" select="$localName" />		                    
				</xsl:call-template>
			</xsl:attribute>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- Generates an FDO Geometric property -->
<xsl:template name="GeometricProperty" >
    <xsl:param name="schemaName" />
	<xsl:param name="elementElem"/>
	<xsl:param name="refUri"/>
	<xsl:param name="refType"/>
	<xsl:param name="baseType"/>
	<xsl:param name="prevTypes"/>
	<xsl:element name="GeometricProperty">
	    <xsl:call-template name="property_attributes">
	        <xsl:with-param name="schemaName" select="$schemaName"/>
	        <xsl:with-param name="elementElem" select="$elementElem"/>
	    </xsl:call-template>
		<xsl:if test="$elementElem/@fdo:geometryReadOnly">
		    <xsl:attribute name="readOnly" >
			    <xsl:value-of select="$elementElem/@fdo:geometryReadOnly" />
		    </xsl:attribute>
		</xsl:if>
		<xsl:if test="$elementElem/@fdov2:geometryReadOnly">
		    <xsl:attribute name="readOnly" >
			    <xsl:value-of select="$elementElem/@fdov2:geometryReadOnly" />
		    </xsl:attribute>
		</xsl:if>
		<xsl:for-each select="$elementElem">
		    <xsl:call-template name="geometricproperty_attributes">
		        <xsl:with-param name="baseType" select="$baseType"/>
		    </xsl:call-template>
		    <xsl:call-template name="element_subelements" />
		</xsl:for-each>
	</xsl:element>
	<!-- Write out any sub-properties, including inherited ones.
	     FDO Geometric properties cannot have sub-properties so these
	     will be promoted to the containing class.
	-->
	<!-- Todo - write out the sub-properties
	<xsl:element name="GeometricSubProperties">
	    <xsl:call-template name="property_attributes">
	        <xsl:with-param name="schemaName" select="$schemaName"/>
	        <xsl:with-param name="elementElem" select="$elementElem"/>
	    </xsl:call-template>
		<xsl:if test="not($prevTypes='')">
		    <xsl:for-each select="$prevTypes">
		        <xsl:call-template name="Properties">
		            <xsl:with-param name="schemaName" select="$schemaName"/>
		        </xsl:call-template>
		    </xsl:for-each>
		</xsl:if>
	</xsl:element>-->
</xsl:template>

<!-- Generates an FDO Geometric property from a Geometric Association -->
<xsl:template name="GeometricAssociation" >
    <xsl:param name="schemaName" />
	<xsl:param name="elementElem"/>
	<xsl:param name="refUri"/>
	<xsl:param name="refType"/>
	<xsl:param name="baseUri"/>
	<xsl:param name="baseType"/>
	<xsl:param name="prevTypes"/>
    <xsl:param name="restrUri" select="''"/>
    <xsl:param name="restrType" select="''"/>

    <xsl:variable name="elemType" select="key('gblTypeKey',concat($refUri,':',$refType))" />
	<xsl:variable name="allTypes" select="$prevTypes|$elemType"/>
	
	<xsl:element name="GeometricAssociation">
	    <xsl:call-template name="property_attributes">
	        <xsl:with-param name="schemaName" select="$schemaName"/>
	        <xsl:with-param name="elementElem" select="$elementElem"/>
	    </xsl:call-template>
		<xsl:for-each select="$elementElem">
		    <xsl:call-template name="element_subelements" />
		</xsl:for-each>
		<!-- Write any association sub-properties (including the 
		     Geometric Property, which is always the first sub-property).
		     Note that FDO will promote these sub-properties to be 
		     properties of the containing class.
		-->
		<!-- Todo pick up sub-properties other than the geometry member
		<xsl:for-each select="$prevTypes">
		    <xsl:call-template name="Properties">
		        <xsl:with-param name="schemaName" select="$schemaName"/>
		    </xsl:call-template>
		</xsl:for-each>-->
        <xsl:variable name="containerUri">
            <xsl:choose>
                <xsl:when test="$restrUri=''">
                    <xsl:value-of select="$baseUri"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$restrUri" />   
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
    
        <xsl:variable name="containerType">
            <xsl:choose>
                <xsl:when test="$restrType=''">
                    <xsl:value-of select="$baseType"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$restrType" />   
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
    
        <xsl:variable name="containerElem" select="key('gblTypeKey',concat($containerUri,':',$containerType))" />

		<xsl:for-each select="$containerElem">
		    <xsl:call-template name="Properties">
		        <xsl:with-param name="schemaName" select="$schemaName"/>
		    </xsl:call-template>
		</xsl:for-each>-->
	</xsl:element>	
</xsl:template>

<!-- Generates an FDO Raster property -->
<xsl:template name="RasterProperty" >
	<xsl:param name="schemaName" />
	<xsl:param name="minOccurs" />
	<xsl:param name="elementElem" />
	<xsl:element name="RasterProperty">
	    <xsl:call-template name="property_attributes">
	        <xsl:with-param name="schemaName" select="$schemaName"/>
	        <xsl:with-param name="elementElem" select="$elementElem"/>
	    </xsl:call-template>
    	<xsl:if test="$minOccurs = '0'" >
           	<xsl:attribute name="nullable" >true</xsl:attribute>
        </xsl:if>
		<xsl:if test="@fdo:readOnly" >
			<xsl:attribute name="readOnly" >
				<xsl:value-of select="@fdo:readOnly" />
			</xsl:attribute>
		</xsl:if>
		<xsl:if test="@fdov2:readOnly" >
			<xsl:attribute name="readOnly" >
				<xsl:value-of select="@fdov2:readOnly" />
			</xsl:attribute>
		</xsl:if>
		<xsl:if test="@fdo:defaultImageXSize" >
			<xsl:attribute name="sizeX" >
				<xsl:value-of select="@fdo:defaultImageXSize" />
			</xsl:attribute>
		</xsl:if>
		<xsl:if test="@fdov2:defaultImageXSize" >
			<xsl:attribute name="sizeX" >
				<xsl:value-of select="@fdov2:defaultImageXSize" />
			</xsl:attribute>
		</xsl:if>
		<xsl:if test="@fdo:defaultImageYSize" >
			<xsl:attribute name="sizeY" >
				<xsl:value-of select="@fdo:defaultImageYSize" />
			</xsl:attribute>
		</xsl:if>
		<xsl:if test="@fdov2:defaultImageYSize" >
			<xsl:attribute name="sizeY" >
				<xsl:value-of select="@fdov2:defaultImageYSize" />
			</xsl:attribute>
		</xsl:if>
        <xsl:if test="@fdo:srsName">
            <xsl:attribute name="srsName" >
              <xsl:value-of select="@fdo:srsName" />
            </xsl:attribute>
        </xsl:if>
        <xsl:if test="@fdov2:srsName">
            <xsl:attribute name="srsName" >
              <xsl:value-of select="@fdov2:srsName" />
            </xsl:attribute>
        </xsl:if>
        <xsl:for-each select="xs:annotation/xs:appinfo/fdo:DefaultDataModel" >
			<xsl:call-template name="RasterPropertyDataModel"/>
		</xsl:for-each>
        <xsl:for-each select="xs:annotation/xs:appinfo/fdov2:DefaultDataModel" >
			<xsl:call-template name="RasterPropertyDataModel"/>
		</xsl:for-each>
		<xsl:call-template name="element_subelements" />
	</xsl:element>
</xsl:template>

<xsl:template name="RasterPropertyDataModel" >
	<xsl:if test="@dataModelType" >
	    <xsl:attribute name="DataModelType" >
			<xsl:value-of select="@dataModelType" />
	    </xsl:attribute>
	</xsl:if>
    <xsl:if test="@dataType" >
        <xsl:attribute name="DataType" >
            <xsl:value-of select="@dataType" />
        </xsl:attribute>
    </xsl:if>
    <xsl:if test="@organization" >
		<xsl:attribute name="Organization" >
		    <xsl:value-of select="@organization" />
		</xsl:attribute>
    </xsl:if>
    <xsl:if test="@bitsPerPixel" >
	    <xsl:attribute name="BitsPerPixel" >
		    <xsl:value-of select="@bitsPerPixel" />
	    </xsl:attribute>
    </xsl:if>
    <xsl:if test="@tileSizeX" >
	    <xsl:attribute name="TileSizeX" >
		    <xsl:value-of select="@tileSizeX" />
	    </xsl:attribute>
    </xsl:if>
    <xsl:if test="@tileSizeY" >
	    <xsl:attribute name="TileSizeY" >
		    <xsl:value-of select="@tileSizeY" />
	    </xsl:attribute>
    </xsl:if>
	</xsl:template>
<!-- The following is used to figure out the minOccurs 
 value for elements that are nested under multiple levels if sequences,
 choices, groups, etc.
-->	
<xsl:template name="resolve_minOccurs" >
	<xsl:param name="minOccurs" />
	<xsl:choose>
		<xsl:when test="local-name='choice'">
			<!-- Elements under a choice are always optional -->
			<xsl:text>0</xsl:text>
		</xsl:when>
    <xsl:when test="@nillable='true'">
      <xsl:text>0</xsl:text>
    </xsl:when>
    <xsl:when test="@nillable='false'">
      <xsl:choose>
        <xsl:when test="$minOccurs='0' or @minOccurs='0'" >
          <xsl:text>0</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>1</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="not(@minOccurs) and not(@nillable) and $element_default_nullability='yes'">
      <xsl:text>0</xsl:text>
    </xsl:when>
    <xsl:otherwise>
			<xsl:choose>
				<xsl:when test="$minOccurs='0' or @minOccurs='0'" >
					<xsl:text>0</xsl:text>
				</xsl:when>
				<xsl:otherwise>
					<!-- An element is mandatory only if all super-elements
					 are mandatory
					-->
					<xsl:text>1</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>	

<!-- The following is used to figure out the maxOccurs 
 value for elements that are nested under multiple levels if sequences,
 choices, groups, etc.
-->	
<xsl:template name="resolve_maxOccurs" >
	<xsl:param name="maxOccurs" />
	<xsl:choose>
		<xsl:when test="$maxOccurs='0'">
			<!-- maxOccurs is 0 if any superelement maxOccurs is 0 -->
			<xsl:text>0</xsl:text>
		</xsl:when>
		<xsl:when test="$maxOccurs='1'">
			<xsl:choose>
				<xsl:when test="not(@maxOccurs)" >
					<!-- maxOccurs defaults to 1 -->
					<xsl:text>1</xsl:text>
				</xsl:when>
				<!-- it is current element's maxOccurs if superelement maxOccurs is 1 -->
				<xsl:when test="@maxOccurs='0' or @maxOccurs='1'" >
					<xsl:value-of select="@maxOccurs"/>
				</xsl:when>
				<xsl:otherwise>
					<!-- no limit if any superelement is unlimited -->
					<xsl:text>unbounded</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:when>
		<xsl:otherwise>
			<xsl:choose>
				<xsl:when test="@maxOccurs='0'" >
					<!-- 0 if current element cannot occur -->
					<xsl:text>0</xsl:text>
				</xsl:when>
				<xsl:otherwise>
					<!-- Otherwise no limit, since no limit on superelement -->
					<xsl:text>unbounded</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>	

<!-- Given a namespace prefix, return the FDO Feature Schema name -->
<xsl:template name="ns_2_schema_name">
    <xsl:param name="ns"/>

    <!-- Get the URI for the prefix -->
    <xsl:variable name="fullName">
        <xsl:call-template name="get_fullname">
            <xsl:with-param name="typeName" select="concat($ns,':test')"/>
	    </xsl:call-template>
	</xsl:variable>
	<xsl:variable name="uriName" >
	    <xsl:value-of select="substring-before($fullName,' ')"/>
	</xsl:variable>
	
    <xsl:choose>
        <xsl:when test="$schema_name_as_prefix='yes' and not($uriName='')">
            <xsl:value-of select="$ns"/>
        </xsl:when>
        <xsl:otherwise>
            <!-- Get the Feature Schema name for the URI -->
            <xsl:call-template name="uri_2_schema_name">
                <xsl:with-param name="uri" select="$uriName"/>
            </xsl:call-template>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<!-- Generate FDO Feature Schema name from uri.
-->
<xsl:template name="uri_2_schema_name">
	<xsl:param name="uri" />

  <xsl:variable name="nameFromPrefix">
    <xsl:if test="$schema_name_as_prefix='yes'">
      <xsl:variable name="uriNamespace" select="namespace::node()[. = $uri]"/>
      <xsl:if test="count($uriNamespace) = 1">
        <xsl:value-of select="local-name($uriNamespace)"/>
      </xsl:if>
    </xsl:if>
  </xsl:variable>
  <xsl:choose>
	    <xsl:when test="not($nameFromPrefix='')">
	        <xsl:value-of select="$nameFromPrefix"/>
	    </xsl:when>
	    <!-- Remove prefix part of uri is prefixed by customer's url.-->
		<xsl:when test="starts-with($uri,$gCustUriPrefix)" >
			<xsl:value-of select="substring-after($uri,$gCustUriPrefix)" />
		</xsl:when>
	    <!-- For backward compatibility. If using default prefix but uri starts with old default prefix,
              then strip out old prefix
        -->
		<xsl:when test="($gCustUriPrefix = $gCustUriPrefixDefault) and starts-with($uri,$gCustUriPrefixDefaultV2)" >
			<xsl:value-of select="substring-after($uri,$gCustUriPrefixDefaultV2)" />
		</xsl:when>
		<!-- Otherwise, just remove the http part -->
		<xsl:otherwise>
			<xsl:value-of select="substring-after($uri,'http://')" />
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- Gets the class type. If the class cannot be determined, the root class and schema
 are also returned ( highest base class not in current document ).
 
 Returns a string of the form <type>:<wkSchema>:<wkClass>:<rootSchema>:<rootClass>
 
 wkSchema and wkClass are the most specialized base class that is well-known (recognized
 by FDO).
 
 If the class type was determined, rootClass and rootSchema are empty: e.g.
 
	"FeatureClass::"
	
 If the type cannot be determined then <type> is set to "ClassDefinition"
-->
<xsl:template name="get_class_type">
	<xsl:param name="schemaName" />
	<xsl:param name="baseUri" />
	<xsl:param name="baseName" />
	<xsl:param name="restriction" />
	<xsl:param name="wkUri" select="''"/>
	<xsl:param name="wkType" select="''"/>
  <xsl:param name="typeElem"/>

  <!-- Determine FDO type for current base type -->
    <xsl:variable name="fdoType">
	    <xsl:choose>
		    <xsl:when test="not($baseName='')" >
                <xsl:call-template name="get_fdo_type">
                    <xsl:with-param name="uriName" select="$baseUri"/>
                    <xsl:with-param name="typeName" select="$baseName"/>
                    <xsl:with-param name="restriction" select="$restriction"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>Class</xsl:otherwise>
        </xsl:choose>
	</xsl:variable>

    <!-- Check if type is FDO class -->
    <xsl:variable name="isClass">
        <xsl:call-template name="fdo_type_is_class">
            <xsl:with-param name="fdoType" select="$fdoType"/>
        </xsl:call-template>
	</xsl:variable>

    <!-- If FDO Type was determined then current base class is well-known -->
    <xsl:variable name="nextWkUri">
        <xsl:choose>
            <xsl:when test="not($fdoType = '') and $wkUri = ''">
                <xsl:value-of select="$baseUri"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$wkUri"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>

    <xsl:variable name="nextWkType">
        <xsl:choose>
            <xsl:when test="not($fdoType = '') and $wkType = ''">
                <xsl:value-of select="$baseName"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$wkType"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>
	<xsl:choose>
    <!-- Resolved to a well-know class so done -->
    <xsl:when test="$isClass='yes'">
      <xsl:value-of select="concat($fdoType,' ',$nextWkUri,' ',$nextWkType,' ',$baseUri,' ',$baseName)"/>
    </xsl:when>
		<xsl:otherwise>
			<!-- Otherwise resolve type from base type of current type -->
      <xsl:choose>
        <xsl:when test="$typeElem">
          <xsl:call-template name="get_baseclass_type" >
            <xsl:with-param name="schemaName">
              <xsl:value-of select="$schemaName" />
            </xsl:with-param>
            <xsl:with-param name="fdoType">
              <xsl:value-of select="$fdoType" />
            </xsl:with-param>
            <xsl:with-param name="baseUri">
              <xsl:value-of select="$baseUri" />
            </xsl:with-param>
            <xsl:with-param name="baseName">
              <xsl:value-of select="$baseName" />
            </xsl:with-param>
            <xsl:with-param name="wkUri">
              <xsl:value-of select="$nextWkUri" />
            </xsl:with-param>
            <xsl:with-param name="wkType">
              <xsl:value-of select="$nextWkType" />
            </xsl:with-param>
            <xsl:with-param name="typeElem" select="$typeElem" />
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="get_baseclass_type" >
            <xsl:with-param name="schemaName">
              <xsl:value-of select="$schemaName" />
            </xsl:with-param>
            <xsl:with-param name="fdoType">
              <xsl:value-of select="$fdoType" />
            </xsl:with-param>
            <xsl:with-param name="baseUri">
              <xsl:value-of select="$baseUri" />
            </xsl:with-param>
            <xsl:with-param name="baseName">
              <xsl:value-of select="$baseName" />
            </xsl:with-param>
            <xsl:with-param name="wkUri">
              <xsl:value-of select="$nextWkUri" />
            </xsl:with-param>
            <xsl:with-param name="wkType">
              <xsl:value-of select="$nextWkType" />
            </xsl:with-param>
            <xsl:with-param name="typeElem" select="key('gblTypeKey',concat($baseUri,':',$baseName))" />
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- Determines the type of the given base class, by walking up
 the class inheritance hierarchy
 -->
<xsl:template name="get_baseclass_type">
	<xsl:param name="schemaName" />
	<xsl:param name="fdoType" />
	<xsl:param name="baseUri" />
	<xsl:param name="baseName" />
	<xsl:param name="wkUri" />
	<xsl:param name="wkType" />
  <xsl:param name="typeElem" />

  <!-- Find the class type based on its URI and local name -->
	<xsl:choose>
		<xsl:when test="$typeElem">
			<!-- base class found, get its base class -->
			<xsl:variable name="qBaseClass" select="$typeElem/xs:complexContent/node()/@base" />
			<xsl:variable name="restriction" select="$typeElem/xs:complexContent/xs:restriction" />
	        <xsl:variable name="fullBaseClass">
	            <xsl:for-each select="$typeElem">
		            <xsl:call-template name="get_fullname" >
			            <xsl:with-param name="typeName" select="$qBaseClass" />
		            </xsl:call-template>			
	            </xsl:for-each>
	        </xsl:variable>
	        <xsl:variable name="nextBaseUri"><xsl:value-of select="substring-before($fullBaseClass,' ')"/></xsl:variable>
	        <xsl:variable name="nextBaseName"><xsl:value-of select="substring-before(substring-after($fullBaseClass,' '),' ')"/></xsl:variable>

			<!-- Get the type of this base class -->
			<xsl:call-template name="get_class_type" >
				<xsl:with-param name="schemaName" select="$schemaName" />
				<xsl:with-param name="baseUri" select="$nextBaseUri" />
				<xsl:with-param name="baseName" select="$nextBaseName" />
				<xsl:with-param name="restriction" select="$restriction" />
				<xsl:with-param name="wkUri" select="$wkUri" />
				<xsl:with-param name="wkType" select="$wkType" />
			</xsl:call-template>
		</xsl:when>
		<!-- base class not found, type must be resolved later -->
		<xsl:otherwise>
		    <xsl:variable name="wellKnown">
		        <xsl:call-template name="base_is_well_known">
				    <xsl:with-param name="schemaName" select="$schemaName" />
				    <xsl:with-param name="baseUri" select="$baseUri" />
				    <xsl:with-param name="baseName" select="$baseName" />
				    <xsl:with-param name="fdoType" select="$fdoType" />
		        </xsl:call-template>
		    </xsl:variable>
		    <xsl:choose>
		        <xsl:when test="$wellKnown='yes'">
        		    <xsl:value-of select="concat('Skip',' ',$wkUri,' ',$wkType,' ',$baseUri,' ',$baseName)"/>
		        </xsl:when>
		        <xsl:otherwise>
        		    <xsl:value-of select="concat('ClassDefinition',' ',$wkUri,' ',$wkType,' ',$baseUri,' ',$baseName)"/>
		        </xsl:otherwise>
		    </xsl:choose>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>	

<!-- Finds the highest base class that is not a restricted complexType.
 -->
<xsl:template name="get_unrestricted_baseclass">
	<xsl:param name="schemaName" />
	<xsl:param name="baseUri" />
	<xsl:param name="baseName" />

	<!-- Find the class type based on its URI and local name -->
	<xsl:variable name="typeElem" select="key('schemaKey',$baseUri)/xs:complexType[@name = $baseName]" />
	<xsl:choose>
		<xsl:when test="$typeElem">
		    <xsl:choose>
		        <xsl:when test="$typeElem/xs:complexContent/xs:restriction">
		            <!-- type is restricted, get its base class -->
	                <xsl:variable name="fullBaseClass">
		                <xsl:call-template name="get_fullname" >
			                <xsl:with-param name="typeName" select="$typeElem/xs:complexContent/xs:restriction/@base" />
		                </xsl:call-template>			
	                </xsl:variable>
	                <xsl:variable name="nextBaseUri"><xsl:value-of select="substring-before($fullBaseClass,' ')"/></xsl:variable>
	                <xsl:variable name="nextBaseName"><xsl:value-of select="substring-after($fullBaseClass,' ')"/></xsl:variable>
			        <!-- Get the type of this base class -->
			        <xsl:call-template name="get_unrestricted_baseclass" >
				        <xsl:with-param name="schemaName" select="$schemaName" />
				        <xsl:with-param name="baseUri" select="$nextBaseUri" />
				        <xsl:with-param name="baseName" select="$nextBaseName" />
			        </xsl:call-template>
		        </xsl:when>
		        <!-- type not restricted, it's the base class we want -->
        		<xsl:otherwise>
        		    <xsl:value-of select="concat($baseUri,' ',$baseName)"/>
        		</xsl:otherwise>
		    </xsl:choose>
			<!-- base class found, get its base class -->
		</xsl:when>
		<!-- base class not found, assume it is not restricted. For FDO XML this is ok since
		     a restricted class and its sub-classes are always in the same schema
		 -->
		<xsl:otherwise><xsl:value-of select="concat($baseUri,' ',$baseName)"/></xsl:otherwise>
	</xsl:choose>
</xsl:template>	

<!-- Determines if base is well known geometric or association type.
 -->
<xsl:template name="base_is_well_known">
	<xsl:param name="schemaName" />
	<xsl:param name="baseUri" />
	<xsl:param name="baseName" />
    <xsl:param name="fdoType" />

	<xsl:choose>
        <xsl:when test="$fdoType='geometricProperty' or $fdoType='associationProperty' or $fdoType='geometricAssociation'">
            <xsl:variable name="elementElem" select="key('gblTypeKey',concat($baseUri,':',$baseName))" />
                <xsl:choose>
                    <xsl:when test="count($elementElem) = 0">
                        <xsl:value-of select="'yes'"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="'no'"/>
                    </xsl:otherwise>
               </xsl:choose>
	      </xsl:when>
	      <xsl:otherwise>
              <xsl:value-of select="'no'"/>
	      </xsl:otherwise>
	 </xsl:choose>
</xsl:template>

<!-- Given an element, returns its type -->
<xsl:template name="get_element_type">
	<xsl:param name="elementElem" />
	<xsl:choose>
		<xsl:when test="$elementElem/@ref" >
	        <xsl:variable name="fullName">
		        <xsl:call-template name="get_fullname" >
			        <xsl:with-param name="typeName" select="$elementElem/@ref" />
		        </xsl:call-template>			
	        </xsl:variable>
	        <xsl:variable name="uriName"><xsl:value-of select="substring-before($fullName,' ')"/></xsl:variable>
	        <xsl:variable name="localName"><xsl:value-of select="substring-before(substring-after($fullName,' '),' ')"/></xsl:variable>
			<xsl:call-template name="get_element_type" >
				<xsl:with-param name="elementElem" select="key('gblTypeKey',concat($uriName,':',$localName))"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
		    <xsl:call-template name="get_fullname" >
			    <xsl:with-param name="typeName" select="$elementElem/@type" />
		    </xsl:call-template>			
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- Given a element, determine if it is well known by FDO. If it is then return its well-known type.
     Otherwise, return type as element name with 'Type' tacked on (as per GML convention).
-->
<xsl:template name="get_element_fdo_type" >
	<xsl:param name="uriName" />
	<xsl:param name="elemName" />

	<xsl:choose>
	    <xsl:when test="$uriName=$gGmlUri">
	        <xsl:choose>
	            <!-- GML 3.1.1 Geometric types and global elements -->
		        <xsl:when test="$elemName='_Geometry'">AbstractGeometryType</xsl:when>
		        <xsl:when test="$elemName='AbstractGeometryCollectionBaseType'">AbstractGeometryCollectionBaseType</xsl:when>

                <!-- Assocations to Geometries -->
		        <xsl:when test="$elemName='GeometryMember'">GeometryPropertyType</xsl:when>
		        <xsl:when test="$elemName='curveMember'">CurvePropertyType</xsl:when>
		        <xsl:when test="$elemName='surfaceMember'">SurfacePropertyType</xsl:when>
		        <xsl:when test="$elemName='multiPointProperty'">MultiPointPropertyType</xsl:when>
		        <xsl:when test="$elemName='multiCurveProperty'">MultiCurvePropertyType</xsl:when>
		        <xsl:when test="$elemName='multiSurfaceProperty'">MultiSurfacePropertyType</xsl:when>

                <!-- GML 2.1 Assocations to Geometries, deprecated in 3.0 -->
		        <xsl:when test="$elemName='lineStringProperty'">LineStringPropertyType</xsl:when>
		        <xsl:when test="$elemName='lineStringMember'">LineStringMemberType</xsl:when>
		        <xsl:when test="$elemName='polygonProperty'">PolygonPropertyType</xsl:when>
		        <xsl:when test="$elemName='polygonMember'">PolygonMemberType</xsl:when>

                <!-- Geometric Primitives -->
		        <xsl:when test="$elemName='coord'">CoordType</xsl:when>
		        <xsl:when test="$elemName='coordinates'">CoordinatesType</xsl:when>

                <!-- Features, collections and associations -->
				<!-- Classes based on AbstractFeatureType are Feature Classes -->
				<xsl:when test="$elemName='_Feature'">AbstractFeatureType</xsl:when>
		        <xsl:when test="$elemName='_FeatureCollection'">AbstractFeatureCollectionType</xsl:when>
		        <xsl:when test="$elemName='featureMember'">FeatureAssociationType</xsl:when>
		        <xsl:otherwise>
		            <xsl:value-of select="concat($elemName,'Type')"/>
		        </xsl:otherwise>
  	        </xsl:choose>
	    </xsl:when>
	    <xsl:when test="$uriName=$gWfsUri">
            <xsl:choose>
		        <xsl:when test="$elemName='featureMember'">FeatureAssociationType</xsl:when>
		        <xsl:when test="$elemName='FeatureCollection'">AbstractFeatureCollectionBaseType</xsl:when>
		        <xsl:otherwise>
		            <xsl:value-of select="concat($elemName,'Type')"/>
		        </xsl:otherwise>
            </xsl:choose>
	    </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="concat($elemName,'Type')"/>
        </xsl:otherwise>
	</xsl:choose>
</xsl:template>


<!-- Given a gml type, return its corresponding FDO Data Type.
 Returns empty content if the gmlType does not map onto a data, geometric or 
 assocation property type.
-->
<xsl:template name="get_fdo_type" >
	<xsl:param name="uriName" />
	<xsl:param name="typeName" />
	<xsl:param name="restriction" select="/.."/>

	<xsl:choose>
		<!-- If base class is restricted then class might be a network type class -->
		<!-- Classes that have a gml:maximalComplex must be networks or layers -->
		<xsl:when test="$restriction and $restriction/xs:sequence/xs:element[@ref='gml:maximalComplex']">
            <xsl:choose>
                <!-- In FDO XML, networks can't be sub-networks -->
                <xsl:when test="$restriction/xs:sequence/xs:element[@ref='gml:superComplex' and @maxOccurs='0']">
		            <xsl:value-of select="'NetworkClass'"/>
		        </xsl:when>
                <!-- but a layer can be a subcomplex of a network -->
                <xsl:otherwise>
		            <xsl:value-of select="'NetworkLayerClass'"/>
		        </xsl:otherwise>
            </xsl:choose>
		</xsl:when>
		<!-- Classes that have a gml:isolated must be network primitive classes -->
		<xsl:when test="$restriction and $restriction/xs:sequence/xs:element[@ref='gml:isolated']">
            <xsl:choose>
       			<!-- Only links can have directed faces -->
                <xsl:when test="$restriction/xs:sequence/xs:element[@ref='gml:directedFace']">
		            <xsl:value-of select="'NetworkLinkFeatureClass'"/>
		        </xsl:when>
            	<!-- if no directed face element then must be a node -->
                <xsl:otherwise>NetworkNodeFeatureClass</xsl:otherwise>
            </xsl:choose>
    	</xsl:when>
	    <xsl:when test="$uriName=$gXsUri">
	        <xsl:choose>
		        <xsl:when test="$typeName='boolean'">boolean</xsl:when>
		        <xsl:when test="$typeName='dateTime'">datetime</xsl:when>
		        <xsl:when test="$typeName='double'">double</xsl:when>
		        <xsl:when test="$typeName='float'">single</xsl:when>
		        <xsl:when test="$typeName='hexBinary'">blob</xsl:when>
		        <xsl:when test="$typeName='string'">string</xsl:when>
		        <xsl:when test="$typeName='decimal'">decimal</xsl:when>
		        <xsl:when test="$typeName='int' or $typeName='integer' or $typeName='nonPositiveInteger' or $typeName='negativeInteger' or $typeName='positiveInteger' or $typeName='nonNegativeInteger'">int64</xsl:when>
		        <xsl:when test="$typeName='unsignedInt'">int64</xsl:when>
		        <xsl:when test="$typeName='short'">int16</xsl:when>
		        <xsl:when test="$typeName='unsignedShort'">int32</xsl:when>
		        <xsl:when test="$typeName='ID' or $typeName='anyURI'">string</xsl:when>
				<!-- Classes based on Any are Non-Feature Classes -->
				<xsl:when test="$typeName = 'anyType'">Class</xsl:when>
	        </xsl:choose>
	    </xsl:when>
	    <xsl:when test="$uriName=$gFdoUri">
	        <xsl:choose>
		        <xsl:when test="$typeName='byte'">byte</xsl:when>
		        <xsl:when test="$typeName='int16'">int16</xsl:when>
		        <xsl:when test="$typeName='int32'">int32</xsl:when>
		        <xsl:when test="$typeName='int64'">int64</xsl:when>
		        <xsl:when test="$typeName='clob'">clob</xsl:when>
		        <xsl:when test="$typeName='RasterPropertyType'">rasterProperty</xsl:when>
				<!-- Classes based on TopoComplexType are Network Classes -->
				<xsl:when test="$typeName='TopoComplexType'">NetworkClass</xsl:when>
				<!-- Classes based on NetworkLayerType are Network Layer Classes -->
				<xsl:when test="$typeName='NetworkLayerType'">NetworkLayerClass</xsl:when>
				<!-- Classes based on EdgeType are Link Feature Classes -->
				<xsl:when test="$typeName='EdgeType'">NetworkLinkFeatureClass</xsl:when>
				<!-- Classes based on NodeType are Node Feature Classes -->
				<xsl:when test="$typeName='NodeType'">NetworkNodeFeatureClass</xsl:when>
				<!-- Classes based on fdo:ClassType are non-Feature Classes -->
				<xsl:when test="$typeName='ClassType'">Class</xsl:when>
	        </xsl:choose>
	    </xsl:when>
	    <xsl:when test="$uriName=$gFdoV2Uri">
	        <xsl:choose>
		        <xsl:when test="$typeName='byte'">byte</xsl:when>
		        <xsl:when test="$typeName='int16'">int16</xsl:when>
		        <xsl:when test="$typeName='int32'">int32</xsl:when>
		        <xsl:when test="$typeName='int64'">int64</xsl:when>
		        <xsl:when test="$typeName='clob'">clob</xsl:when>
		        <xsl:when test="$typeName='RasterPropertyType'">rasterProperty</xsl:when>
				<!-- Classes based on TopoComplexType are Network Classes -->
				<xsl:when test="$typeName='TopoComplexType'">NetworkClass</xsl:when>
				<!-- Classes based on NetworkLayerType are Network Layer Classes -->
				<xsl:when test="$typeName='NetworkLayerType'">NetworkLayerClass</xsl:when>
				<!-- Classes based on EdgeType are Link Feature Classes -->
				<xsl:when test="$typeName='EdgeType'">NetworkLinkFeatureClass</xsl:when>
				<!-- Classes based on NodeType are Node Feature Classes -->
				<xsl:when test="$typeName='NodeType'">NetworkNodeFeatureClass</xsl:when>
				<!-- Classes based on fdo:ClassType are non-Feature Classes -->
				<xsl:when test="$typeName='ClassType'">Class</xsl:when>
	        </xsl:choose>
	    </xsl:when>
	    <xsl:when test="$uriName=$gGmlUri">
	        <xsl:choose>
	            <!-- GML 3.1.1 Geometric types and global elements -->
		        <xsl:when test="$typeName='AbstractGeometryType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='PointType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='LineStringType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='PolygonType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='LinearRingType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='CompositeCurveType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='CompositeSurfaceType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='CurveType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='LineStringSegmentType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='ArcType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='CircleType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='ArcByCenterPointType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='CircleByCenterPointType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='TriangleType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='RectangleType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='RingType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='BoxType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='MultiPointType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='MultiCurveType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='MultiSurfaceType'">geometricProperty</xsl:when>
	            
	            <!-- GML 2.1 Geometric types and global elements deprecated in 3.0 -->
		        <xsl:when test="$typeName='MultiLineStringType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='MultiPolygonType'">geometricProperty</xsl:when>
		        <xsl:when test="$typeName='AbstractGeometryCollectionBaseType'">geometricProperty</xsl:when>

                <!-- Assocations to Geometries -->
		        <xsl:when test="$typeName='GeometryPropertyType'">geometricAssociation</xsl:when>
		        <xsl:when test="$typeName='PointPropertyType'or $typeName='PointMemberType'">geometricAssociation</xsl:when>
		        <xsl:when test="$typeName='CurvePropertyType'">geometricAssociation</xsl:when>
		        <xsl:when test="$typeName='SurfacePropertyType'">geometricAssociation</xsl:when>
		        <xsl:when test="$typeName='LinearRingPropertyType'">geometricAssociation</xsl:when>
		        <xsl:when test="$typeName='GeometricComplexPropertyType'">geometricAssociation</xsl:when>
		        <xsl:when test="$typeName='RingPropertyType'">geometricAssociation</xsl:when>
		        <xsl:when test="$typeName='MultiGeometryPropertyType'">geometricAssociation</xsl:when>
		        <xsl:when test="$typeName='MultiPointPropertyType'">geometricAssociation</xsl:when>
		        <xsl:when test="$typeName='MultiCurvePropertyType'">geometricAssociation</xsl:when>
		        <xsl:when test="$typeName='MultiSurfacePropertyType'">geometricAssociation</xsl:when>

                <!-- GML 2.1 Assocations to Geometries, deprecated in 3.0 -->
		        <xsl:when test="$typeName='GeometryAssociationType'">geometricAssociation</xsl:when>
		        <xsl:when test="$typeName='LineStringPropertyType'or $typeName='LineStringMemberType'">geometricAssociation</xsl:when>
		        <xsl:when test="$typeName='PolygonPropertyType'or $typeName='PolygonMemberType'">geometricAssociation</xsl:when>
		        <xsl:when test="$typeName='MultiLineStringPropertyType'">geometricAssociation</xsl:when>
		        <xsl:when test="$typeName='MultiPolygonPropertyType'">geometricAssociation</xsl:when>
		        <xsl:when test="$typeName='LinearRingMemberType'">geometricAssociation</xsl:when>

                <!-- Geometric Primitives -->
		        <xsl:when test="$typeName='CoordType'">Class</xsl:when>
		        <xsl:when test="$typeName='CoordinatesType'">Class</xsl:when>

                <!-- Features, collections and associations -->
				<!-- Classes based on AbstractFeatureType are Feature Classes -->
				<xsl:when test="$typeName='AbstractFeatureType'">FeatureClass</xsl:when>
		        <xsl:when test="$typeName='AbstractFeatureCollectionBaseType'">FeatureClass</xsl:when>
		        <xsl:when test="$typeName='FeaturePropertyType'">associationProperty</xsl:when>
		        <xsl:when test="$typeName='FeatureAssociationType'">associationProperty</xsl:when>

                <!-- Topologies -->
				<!-- Classes based on TopoComplexType are Network Classes -->
				<xsl:when test="$typeName='TopoComplexType'">NetworkClass</xsl:when>
				<!-- Classes based on EdgeType are Link Feature Classes -->
				<xsl:when test="$typeName='EdgeType'">NetworkLinkFeatureClass</xsl:when>
				<!-- Classes based on NodeType are Node Feature Classes -->
				<xsl:when test="$typeName='NodeType'">NetworkNodeFeatureClass</xsl:when>
				<xsl:when test="$typeName='topoPrimitiveMemberType'">Skip</xsl:when>
				<xsl:when test="$typeName='TopoComplexMemberType'">Skip</xsl:when>
				<xsl:when test="$typeName='FeaturePropertyType'">Skip</xsl:when>
	        </xsl:choose>
	    </xsl:when>
	</xsl:choose>
</xsl:template>

<!-- Determines if an FDO type corresponds to a class, or is just a data or 
     Geometric type
-->
<xsl:template name="fdo_type_is_class">
	<xsl:param name="fdoType"/>
	
	<xsl:choose>
        <xsl:when test="$fdoType='Skip' or $fdoType='Class' or $fdoType='FeatureClass' or $fdoType='NetworkClass' or $fdoType='NetworkLayerClass' or $fdoType='NetworkLinkFeatureClass' or $fdoType='NetworkNodeFeatureClass'">
            <xsl:value-of select="'yes'"/>
        </xsl:when>
        <xsl:otherwise>no</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- Given a GML geometry type or global element, return the FDO geometryTypes.
-->

<xsl:template name="get_geometry_types" >
	<xsl:param name="uriName" select="$gGmlUri"/>
	<xsl:param name="typeName" />

    <xsl:if test="$uriName=$gGmlUri">
	    <xsl:choose>
	        <!-- GML 3.1.1 Geometric types and global elements -->
	        <xsl:when test="$typeName='AbstractGeometryType' or $typeName='_Geometry'">point curve surface solid</xsl:when>
	        <xsl:when test="$typeName='PointType' or $typeName='Point'">point</xsl:when>
	        <xsl:when test="$typeName='LineStringType' or $typeName='LineString'">curve</xsl:when>
	        <xsl:when test="$typeName='PolygonType' or $typeName='Polygon'">surface</xsl:when>
	        <xsl:when test="$typeName='LinearRingType' or $typeName='LinearRing'">surface</xsl:when>
	        <xsl:when test="$typeName='CompositeCurveType' or $typeName='CompositeCurve'">curve</xsl:when>
	        <xsl:when test="$typeName='CompositeSurfaceType' or $typeName='CompositeSurface'">surface</xsl:when>
	        <xsl:when test="$typeName='CurveType' or $typeName='Curve'">curve</xsl:when>
	        <xsl:when test="$typeName='LineStringSegmentType' or $typeName='LineStringSegment'">curve</xsl:when>
	        <xsl:when test="$typeName='ArcType' or $typeName='Arc'">curve</xsl:when>
	        <xsl:when test="$typeName='CircleType' or $typeName='Circle'">curve</xsl:when>
	        <xsl:when test="$typeName='ArcByCenterPointType' or $typeName='ArcByCenterPoint'">curve</xsl:when>
	        <xsl:when test="$typeName='CircleByCenterPointType' or $typeName='CircleByCenterPoint'">curve</xsl:when>
	        <xsl:when test="$typeName='TriangleType' or $typeName='Triangle'">surface</xsl:when>
	        <xsl:when test="$typeName='RectangleType' or $typeName='Rectangle'">surface</xsl:when>
	        <xsl:when test="$typeName='RingType' or $typeName='Ring'">surface</xsl:when>
	        <xsl:when test="$typeName='MultiPointType' or $typeName='MultiPoint'">point</xsl:when>
	        <xsl:when test="$typeName='MultiCurveType' or $typeName='MultiCurve'">curve</xsl:when>
	        <xsl:when test="$typeName='MultiSurfaceType' or $typeName='MultiSurface'">surface</xsl:when>
	           
	        <!-- GML 2.1 Geometric types and global elements deprecated in 3.0 -->
	        <xsl:when test="$typeName='MultiLineStringType' or $typeName='MultiLineString'">curve</xsl:when>
	        <xsl:when test="$typeName='MultiPolygonType' or $typeName='MultiPolygon'">surface</xsl:when>

            <!-- Assocations to Geometries -->
	        <xsl:when test="$typeName='GeometryPropertyType' or $typeName='GeometryMember'">point curve surface solid</xsl:when>
	        <xsl:when test="$typeName='PointPropertyType' or $typeName='pointProperty' or $typeName='pointMember'">point</xsl:when>
	        <xsl:when test="$typeName='CurvePropertyType' or $typeName='curveProperty' or $typeName='curveMember'">curve</xsl:when>
	        <xsl:when test="$typeName='SurfacePropertyType' or $typeName='surfaceProperty' or $typeName='surfaceMember'">surface</xsl:when>
	        <xsl:when test="$typeName='LinearRingPropertyType'">surface</xsl:when>
	        <xsl:when test="$typeName='GeometricComplexPropertyType'">point curve surface solid</xsl:when>
	        <xsl:when test="$typeName='RingPropertyType'">surface</xsl:when>
	        <xsl:when test="$typeName='MultiGeometryPropertyType'">point curve surface solid</xsl:when>
	        <xsl:when test="$typeName='MultiPointPropertyType' or $typeName='multiPointProperty'">point</xsl:when>
	        <xsl:when test="$typeName='MultiCurvePropertyType' or $typeName='multiCurveProperty' or $typeName='curveMember'">curve</xsl:when>
	        <xsl:when test="$typeName='MultiSurfacePropertyType' or $typeName='multiSurfaceProperty'">surface</xsl:when>

            <!-- GML 2.1 Assocations to Geometries, deprecated in 3.0 -->
	        <xsl:when test="$typeName='LineStringPropertyType' or $typeName='lineStringProperty' or $typeName='lineStringMember'">curve</xsl:when>
	        <xsl:when test="$typeName='PolygonPropertyType' or $typeName='polygonProperty' or $typeName='polygonMember'">surface</xsl:when>
	        <xsl:when test="$typeName='MultiLineStringPropertyType'">curve</xsl:when>
	        <xsl:when test="$typeName='MultiPolygonPropertyType'">surface</xsl:when>

        </xsl:choose>
	</xsl:if>
</xsl:template>


 <xsl:template name="get_new_geometry_types" >
    <xsl:param name="uriName" select="$gGmlUri"/>
    <xsl:param name="typeName" />

    <xsl:if test="$uriName=$gGmlUri">
        <xsl:choose>
            <!-- GML 3.1.1 Geometric types and global elements -->
            <xsl:when test="$typeName='AbstractGeometryType' or $typeName='_Geometry'">point multipoint linestring multilinestring curvestring multicurvestring polygon multipolygon curvepolygon multicurvepolygon</xsl:when>
            <xsl:when test="$typeName='PointType' or $typeName='Point'">point</xsl:when>
            <xsl:when test="$typeName='LineStringType' or $typeName='LineString'">linestring</xsl:when>
            <xsl:when test="$typeName='PolygonType' or $typeName='Polygon'">polygon</xsl:when>
            <xsl:when test="$typeName='LinearRingType' or $typeName='LinearRing'">polygon</xsl:when>
            <xsl:when test="$typeName='CompositeCurveType' or $typeName='CompositeCurve'">multicurvestring</xsl:when>
            <xsl:when test="$typeName='CompositeSurfaceType' or $typeName='CompositeSurface'">polygon</xsl:when>
            <xsl:when test="$typeName='CurveType' or $typeName='Curve'">curvestring</xsl:when>
            <xsl:when test="$typeName='LineStringSegmentType' or $typeName='LineStringSegment'">linestring</xsl:when>
            <xsl:when test="$typeName='ArcType' or $typeName='Arc'">curvestring</xsl:when>
            <xsl:when test="$typeName='CircleType' or $typeName='Circle'">curvestring</xsl:when>
            <xsl:when test="$typeName='ArcByCenterPointType' or $typeName='ArcByCenterPoint'">curvestring</xsl:when>
            <xsl:when test="$typeName='CircleByCenterPointType' or $typeName='CircleByCenterPoint'">curvestring</xsl:when>
            <xsl:when test="$typeName='TriangleType' or $typeName='Triangle'">polygon</xsl:when>
            <xsl:when test="$typeName='RectangleType' or $typeName='Rectangle'">polygon</xsl:when>
            <xsl:when test="$typeName='RingType' or $typeName='Ring'">polygon</xsl:when>
            <xsl:when test="$typeName='MultiPointType' or $typeName='MultiPoint'">multipoint</xsl:when>
            <xsl:when test="$typeName='MultiCurveType' or $typeName='MultiCurve'">multicurvestring</xsl:when>
            <xsl:when test="$typeName='MultiSurfaceType' or $typeName='MultiSurface'">multipolygon</xsl:when>

            <!-- GML 2.1 Geometric types and global elements deprecated in 3.0 -->
            <xsl:when test="$typeName='MultiLineStringType' or $typeName='MultiLineString'">multilinestring</xsl:when>
            <xsl:when test="$typeName='MultiPolygonType' or $typeName='MultiPolygon'">multipolygon</xsl:when>

            <!-- Assocations to Geometries -->
            <xsl:when test="$typeName='GeometryPropertyType' or $typeName='GeometryMember'">point multipoint linestring multilinestring curvestring multicurvestring polygon multipolygon curvepolygon multicurvepolygon</xsl:when>
            <xsl:when test="$typeName='PointPropertyType' or $typeName='pointProperty' or $typeName='pointMember'">point</xsl:when>
            <xsl:when test="$typeName='CurvePropertyType' or $typeName='curveProperty' or $typeName='curveMember'">curvestring</xsl:when>
            <xsl:when test="$typeName='SurfacePropertyType' or $typeName='surfaceProperty' or $typeName='surfaceMember'">polygon</xsl:when>
            <xsl:when test="$typeName='LinearRingPropertyType'">polygon</xsl:when>
            <xsl:when test="$typeName='GeometricComplexPropertyType'">point multipoint linestring multilinestring curvestring multicurvestring polygon multipolygon curvepolygon multicurvepolygon</xsl:when>
            <xsl:when test="$typeName='RingPropertyType'">polygon</xsl:when>
            <xsl:when test="$typeName='MultiGeometryPropertyType'">point multipoint linestring multilinestring curvestring multicurvestring polygon multipolygon curvepolygon multicurvepolygon</xsl:when>
            <xsl:when test="$typeName='MultiPointPropertyType' or $typeName='multiPointProperty'">multipoint</xsl:when>
            <xsl:when test="$typeName='MultiCurvePropertyType' or $typeName='multiCurveProperty' or $typeName='curveMember'">multicurvestring</xsl:when>
            <xsl:when test="$typeName='MultiSurfacePropertyType' or $typeName='multiSurfaceProperty'">multipolygon</xsl:when>

            <!-- GML 2.1 Assocations to Geometries, deprecated in 3.0 -->
            <xsl:when test="$typeName='LineStringPropertyType' or $typeName='lineStringProperty' or $typeName='lineStringMember'">linestring</xsl:when>
            <xsl:when test="$typeName='PolygonPropertyType' or $typeName='polygonProperty' or $typeName='polygonMember'">polygon</xsl:when>
            <xsl:when test="$typeName='MultiLineStringPropertyType'">multilinestring</xsl:when>
            <xsl:when test="$typeName='MultiPolygonPropertyType'">multipolygon</xsl:when>

        </xsl:choose>
    </xsl:if>
</xsl:template>

    <!-- Given a qualified name, returns "<uri>:<local-name>:<namespace>" -->
<xsl:template name="get_fullname">
	<xsl:param name="typeName" />
	<xsl:variable name="uriName" >
		<xsl:choose>
			<xsl:when test="contains($typeName,':')">
				<xsl:value-of select="namespace::node()[local-name() = substring-before($typeName,':')]"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="namespace::node()[local-name() = '']"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="localName" >
		<xsl:choose>
			<xsl:when test="contains($typeName,':')">
				<xsl:value-of select="substring-after($typeName,':')"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$typeName"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="prefixName" >
		<xsl:choose>
			<xsl:when test="contains($typeName,':')">
				<xsl:value-of select="substring-before($typeName,':')"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="''"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:value-of select="concat( $uriName, concat( ' ', concat( $localName, concat( ' ', $prefixName ) ) ) )" />
</xsl:template>

<!-- Writes the attributes and sub-elements for the geometryProperty of a 
 feature class.
--> 
<xsl:template name="geometry_property">
	<!--<xsl:call-template name="class_attributes" />-->
	<!-- Don't generate geometric property if geometry property is inherited.
	     It has already been generated in this case.
	-->
	<xsl:if test="@fdo:geometryName and (@fdo:geometricTypes or @fdo:geometryTypes) and not(@fdo:geometryInherited='true')">
		<xsl:element name="GeometricProperty">
			<xsl:attribute name="name">
				<xsl:value-of select="@fdo:geometryName" />
			</xsl:attribute>
			<xsl:if test="@fdo:geometryReadOnly">
				<xsl:attribute name="readOnly" >
    				<xsl:value-of select="@fdo:geometryReadOnly" />
				</xsl:attribute>
			</xsl:if>
			<xsl:call-template name="geometricproperty_attributes" />
			<xsl:call-template name="element_subelements">
				<xsl:with-param name="lastAnno" select="'true'"/>
			</xsl:call-template>
		</xsl:element>
	</xsl:if>
	<xsl:if test="@fdov2:geometryName and @fdov2:geometricTypes and not(@fdov2:geometryInherited='true')">
		<xsl:element name="GeometricProperty">
			<xsl:attribute name="name">
				<xsl:value-of select="@fdov2:geometryName" />
			</xsl:attribute>
			<xsl:if test="@fdov2:geometryReadOnly">
				<xsl:attribute name="readOnly" >
    				<xsl:value-of select="@fdov2:geometryReadOnly" />
				</xsl:attribute>
			</xsl:if>
			<xsl:call-template name="geometricproperty_attributes" />
			<xsl:call-template name="element_subelements">
				<xsl:with-param name="lastAnno" select="'true'"/>
			</xsl:call-template>
		</xsl:element>
	</xsl:if>
</xsl:template>

<xsl:template name="get_minmax_facet">
    <xsl:param name="types"/>
    <xsl:param name="facetName"/>
    <xsl:param name="min"/>
    <xsl:param name="currPos" select="1"/>
    <xsl:param name="prevVal" select="''"/>
    
    <xsl:variable name="currVal" select="$types[position()=$currPos]/xs:restriction/node()[local-name()=$facetName]/@value"/>
    <xsl:variable name="nextVal">
        <xsl:choose>
            <xsl:when test="($prevVal = '') or ($min='y' and $currVal &gt; $prevVal) or (not($min='y') and $currVal &lt; $prevVal)">
                <xsl:value-of select="$currVal"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$prevVal"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>
    
    <xsl:choose>
        <xsl:when test="$currPos &lt; count($types)">
            <xsl:call-template name="get_minmax_facet">
                <xsl:with-param name="types" select="$types"/>
                <xsl:with-param name="min" select="$min"/>
                <xsl:with-param name="facetName" select="$facetName"/>
                <xsl:with-param name="currPos" select="$currPos + 1"/>
                <xsl:with-param name="prevVal" select="$nextVal"/>
            </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="$nextVal"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="get_enum_facets">
    <xsl:param name="types"/>
    <xsl:param name="currPos" select="1"/>
    <xsl:param name="prevEnums" select="/.." />
    
    <xsl:variable name="currEnums" select="$types[position()=$currPos]/xs:restriction/xs:enumeration/@value"/>

    <xsl:variable name="nextEnums" select="$currEnums">
        <xsl:choose>
            <xsl:when test="count($currEnums) = 0 and count($prevEnums) = 0">
                <xsl:value-of select="/.."/>
            </xsl:when>
            <xsl:when test="count($currEnums) = 0">
                <xsl:value-of select="$prevEnums"/>
            </xsl:when>
            <xsl:when test="count($prevEnums) = 0">
                <xsl:value-of select="$currEnums"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:choose>
                    <xsl:when test="count($currEnums) &lt; count($prevEnums)">
                        <xsl:value-of select="$currEnums"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="$prevEnums"/>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>

    <xsl:choose>
        <xsl:when test="$currPos &lt; count($types)">
            <xsl:call-template name="get_enum_facets">
                <xsl:with-param name="types" select="$types"/>
                <xsl:with-param name="currPos" select="$currPos + 1"/>
                <xsl:with-param name="prevEnums" select="$nextEnums"/>
            </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
            <xsl:choose>
                <xsl:when test="count($nextEnums) = 0">
                    <xsl:value-of select="/.."/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$nextEnums"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<!-- writes the attributes of a geometric property -->
<xsl:template name="geometricproperty_attributes">
    <xsl:param name="baseType"/>
	<xsl:if test="@fdo:hasMeasure">
		<xsl:attribute name="hasMeasure" >
			<xsl:value-of select="@fdo:hasMeasure" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="@fdov2:hasMeasure">
		<xsl:attribute name="hasMeasure" >
			<xsl:value-of select="@fdov2:hasMeasure" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="@fdo:hasElevation">
		<xsl:attribute name="hasElevation" >
			<xsl:value-of select="@fdo:hasElevation" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="@fdov2:hasElevation">
		<xsl:attribute name="hasElevation" >
			<xsl:value-of select="@fdov2:hasElevation" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="@fdo:srsName">
		<xsl:attribute name="associatedSCName" >
			<xsl:value-of select="@fdo:srsName" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="@fdov2:srsName">
		<xsl:attribute name="associatedSCName" >
			<xsl:value-of select="@fdov2:srsName" />
		</xsl:attribute>
	</xsl:if>
	<xsl:choose>
	    <xsl:when test="@fdo:geometricTypes">
		    <xsl:element name="GeometricTypes">
			    <xsl:call-template name="geometric_types">
				    <xsl:with-param name="geometricTypes" select="@fdo:geometricTypes" />
			    </xsl:call-template>
		    </xsl:element>
	    </xsl:when>
	    <xsl:when test="@fdov2:geometricTypes">
		    <xsl:element name="GeometricTypes">
			    <xsl:call-template name="geometric_types">
				    <xsl:with-param name="geometricTypes" select="@fdov2:geometricTypes" />
			    </xsl:call-template>
		    </xsl:element>
	    </xsl:when>
	    <xsl:when test="$baseType">
		    <xsl:element name="GeometricTypes">
			    <xsl:call-template name="geometric_types">
				    <xsl:with-param name="geometricTypes">
				        <xsl:call-template name="get_geometry_types">
				            <xsl:with-param name="typeName" select="$baseType"/>
				        </xsl:call-template>
				    </xsl:with-param>
			    </xsl:call-template>
		    </xsl:element>
	    </xsl:when>
	</xsl:choose>
    <xsl:choose>
        <xsl:when test="@fdo:geometryTypes">
            <xsl:element name="GeometryTypes">
                <xsl:call-template name="geometry_types">
                    <xsl:with-param name="geometryTypes" select="@fdo:geometryTypes" />
                </xsl:call-template>
            </xsl:element>
        </xsl:when>
        <xsl:when test="$baseType">
            <xsl:if test="not ($baseType='AbstractGeometryType' or $baseType='_Geometry')">
                <xsl:element name="GeometryTypes">
                    <xsl:call-template name="geometry_types">
                        <xsl:with-param name="geometryTypes">
                            <xsl:call-template name="get_new_geometry_types">
                                <xsl:with-param name="typeName" select="$baseType"/>
                            </xsl:call-template>
                        </xsl:with-param>
                    </xsl:call-template>
                </xsl:element>
            </xsl:if>
        </xsl:when>
    </xsl:choose>
</xsl:template>

<!-- Writes the geometric types for a geometric property. Parses them from 
 a list attribute to a sequence of sub-elements
-->
<xsl:template name="geometric_types">
	<xsl:param name="geometricTypes" />
	<xsl:choose>
		<xsl:when test="contains($geometricTypes,' ')" >
			<xsl:call-template name="geometric_type">
				<xsl:with-param name="geometricType" select="substring-before($geometricTypes,' ')" />
			</xsl:call-template>
			<xsl:call-template name="geometric_types">
				<xsl:with-param name="geometricTypes" select="substring-after($geometricTypes,' ')" />
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
			<xsl:call-template name="geometric_type">
				<xsl:with-param name="geometricType" select="$geometricTypes" />
			</xsl:call-template>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template name="geometric_type">
	<xsl:param name="geometricType" />
	<xsl:if test="not($geometricType = '')">
		<xsl:element name="GeometricType">
			<xsl:value-of select="$geometricType" />
		</xsl:element>
	</xsl:if>
</xsl:template>

<xsl:template name="geometry_types">
    <xsl:param name="geometryTypes" />
    <xsl:choose>
        <xsl:when test="contains($geometryTypes,' ')" >
            <xsl:call-template name="geometry_type">
                <xsl:with-param name="geometryType" select="substring-before($geometryTypes,' ')" />
            </xsl:call-template>
            <xsl:call-template name="geometry_types">
                <xsl:with-param name="geometryTypes" select="substring-after($geometryTypes,' ')" />
            </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
            <xsl:call-template name="geometry_type">
                <xsl:with-param name="geometryType" select="$geometryTypes" />
            </xsl:call-template>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="geometry_type">
    <xsl:param name="geometryType" />
    <xsl:if test="not($geometryType = '')">
        <xsl:element name="GeometryType">
            <xsl:value-of select="$geometryType" />
        </xsl:element>
    </xsl:if>
</xsl:template>

    <!-- translates association property attributes from external to internal format.
     elementElem must be an association property.
-->
<xsl:template name="association_attributes">
	<xsl:param name="schemaName" />
	<xsl:param name="elementElem" select="."/>

	<xsl:if test="$elementElem/@fdo:multiplicity" >
		<xsl:attribute name="multiplicity" >
			<xsl:value-of select="$elementElem/@fdo:multiplicity" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="$elementElem/@fdov2:multiplicity" >
		<xsl:attribute name="multiplicity" >
			<xsl:value-of select="$elementElem/@fdov2:multiplicity" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="$elementElem/@fdo:deleteRule" >
		<xsl:attribute name="deleteRule" >
			<xsl:value-of select="$elementElem/@fdo:deleteRule" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="$elementElem/@fdov2:deleteRule" >
		<xsl:attribute name="deleteRule" >
			<xsl:value-of select="$elementElem/@fdov2:deleteRule" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="$elementElem/@fdo:lockCascade" >
		<xsl:attribute name="lockCascade" >
			<xsl:value-of select="$elementElem/@fdo:lockCascade" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="$elementElem/@fdov2:lockCascade" >
		<xsl:attribute name="lockCascade" >
			<xsl:value-of select="$elementElem/@fdov2:lockCascade" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="$elementElem/@fdo:reverseName" >
		<xsl:attribute name="reverseName" >
			<xsl:value-of select="$elementElem/@fdo:reverseName" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="$elementElem/@fdov2:reverseName" >
		<xsl:attribute name="reverseName" >
			<xsl:value-of select="$elementElem/@fdov2:reverseName" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="$elementElem/@fdo:readOnly" >
		<xsl:attribute name="readOnly" >
			<xsl:value-of select="$elementElem/@fdo:readOnly" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="$elementElem/@fdov2:readOnly" >
		<xsl:attribute name="readOnly" >
			<xsl:value-of select="$elementElem/@fdov2:readOnly" />
		</xsl:attribute>
	</xsl:if>
</xsl:template>

<!-- Write the general property attributes -->
<xsl:template name="property_attributes">
	<xsl:param name="schemaName" />
	<xsl:param name="elementElem" />

    <xsl:variable name="propName">
        <xsl:choose>
            <xsl:when test="$elementElem/@name">
    			<xsl:value-of select="$elementElem/@name" />
            </xsl:when>
            <xsl:otherwise>
    			<xsl:value-of select="translate($elementElem/@ref,':','/')" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>
	<xsl:attribute name="name">
		<xsl:value-of select="$propName" />
	</xsl:attribute>
</xsl:template>
	
<!-- write the layer schema and name attributes for the current network -->
<xsl:template name="network_attributes">
    <xsl:param name="schemaName"/>
    <xsl:param name="baseUri"/>
    <xsl:param name="baseClass"/>
	<!-- the layer is defined by the network's restricted base class -->
	<xsl:variable name="networkName">
	    <xsl:call-template name="type_to_class">
	        <xsl:with-param name="type" select="@name"/>
	    </xsl:call-template>
	</xsl:variable>
	<xsl:variable name="restrictedClass" select="key('schemaKey',$baseUri)/xs:complexType[(@name = $baseClass) and xs:complexContent/xs:restriction]" />
    <xsl:if test="$restrictedClass">
        <!-- layer pointed to by the layerMember element in the restricted class -->
        <xsl:variable name="layerMember" select="concat($schemaName,':',$networkName,'_LayerMember')"/>
        <xsl:for-each select="$restrictedClass/xs:complexContent/xs:restriction/xs:sequence/xs:element[@ref = $layerMember]">
            <xsl:if test="not(@maxOccurs = 0)">
			    <xsl:variable name="fullName">
				    <xsl:call-template name="get_fullname" >
					    <xsl:with-param name="typeName" select="@ref" />
				    </xsl:call-template>			
			    </xsl:variable>
    			<xsl:variable name="uriName"><xsl:value-of select="substring-before($fullName,' ')"/></xsl:variable>
			    <xsl:variable name="localName"><xsl:value-of select="substring-before(substring-after($fullName,' '),' ')"/></xsl:variable>
            	<!-- Find the global member property element referenced by the layer member -->
            	<xsl:variable name="memberElement" select="key('schemaKey',$uriName)/xs:element[(@name = $localName)]" />
                <xsl:if test="$memberElement">
                    <xsl:variable name="fullType">
				        <xsl:call-template name="get_fullname" >
    				        <xsl:with-param name="typeName" select="$memberElement/@type" />
	    	            </xsl:call-template>			
		            </xsl:variable>
			        <xsl:variable name="uriType"><xsl:value-of select="substring-before($fullType,' ')"/></xsl:variable>
		            <xsl:variable name="localType" select="substring-before(substring-after($fullType,' '),' ')"/>
        	        <!-- Find the member property type for the above global element -->
        	        <xsl:variable name="memberType" select="key('schemaKey',$uriType)/xs:complexType[(@name = $localType)]" />
                    <xsl:for-each select="$memberType">
                        <!-- layer element is referenced by the first sub-element of the member property type -->
                        <xsl:variable name="layerElemFullName">
                            <xsl:call-template name="get_fullname" >
    	                        <xsl:with-param name="typeName" select="xs:complexContent/xs:restriction//xs:sequence/xs:element[position() = 1]/@ref" />
	                        </xsl:call-template>			
                        </xsl:variable>
	                    <xsl:variable name="layerElemUri"><xsl:value-of select="substring-before($layerElemFullName,' ')"/></xsl:variable>
                        <xsl:variable name="layerElemName" select="substring-before(substring-after($layerElemFullName,' '),' ')"/>
   	                    <!-- find the layer's element -->
   	                    <xsl:variable name="layerElem" select="key('schemaKey',$layerElemUri)/xs:element[(@name = $layerElemName)]" />
                        <xsl:variable name="layerElemFullType">
                            <xsl:choose>
                                <xsl:when test="$layerElem/@type">
                                    <!-- layer's element found, convert type name to class -->
                                    <xsl:call-template name="get_fullname" >
	                                    <xsl:with-param name="typeName" select="$layerElem/@type" />
                                    </xsl:call-template>
                                </xsl:when>
                                <xsl:otherwise>
                                    <!-- layer's element not found (likely in another schema.
                                         Make best assumption (class name same as element name).
                                    -->
                                    <xsl:value-of select="$layerElemFullName"/>
                                </xsl:otherwise>
                            </xsl:choose>			
                        </xsl:variable>
                        <xsl:variable name="layerElemTypeName" select="substring-before(substring-after($layerElemFullType,' '),' ')"/>
                        <xsl:variable name="layerElemTypeSchema" select="substring-after(substring-after($layerElemFullType,' '),' ')"/>
                        <!-- Found the layer class and schema so write the attributes -->
                        <xsl:if test="$layerElemTypeName">
                            <xsl:attribute name="classSchema">
                                <xsl:value-of select="$layerElemTypeSchema"/>
                            </xsl:attribute>
                            <xsl:attribute name="class">
                                <xsl:choose>
                                    <xsl:when test="$layerElem/@type">
                                        <!-- layer's element found, get its type -->
		                                <xsl:call-template name="type_to_class">
    			                            <xsl:with-param name="type" select="$layerElemTypeName" />		                    
		                                </xsl:call-template>
                                    </xsl:when>
                                    <xsl:otherwise>
                                        <!-- layer's element not found (likely in another schema.
                                            Make best assumption (type name same as element name).
                                        -->
                                        <xsl:value-of select="$layerElemTypeName"/>
                                    </xsl:otherwise>
                                </xsl:choose>			
                            </xsl:attribute>
                        </xsl:if>
                    </xsl:for-each>
                </xsl:if>
            </xsl:if>
        </xsl:for-each>
    </xsl:if>
</xsl:template>

<!-- Writes the association sub-elements specific to a link network feature class -->
<xsl:template name="link_subelements">
    <xsl:param name="schemaName"/>
    <xsl:param name="baseUri"/>
    <xsl:param name="baseClass"/>
    
    <xsl:variable name="referencingElement">
        <xsl:call-template name="type_to_class">
            <xsl:with-param name="type" select="@name"/>
        </xsl:call-template>
    </xsl:variable>
	<!-- Association's are referenced by the link's restricted base class -->
	<xsl:variable name="restrictedClass" select="key('schemaKey',$baseUri)/xs:complexType[(@name = $baseClass) and xs:complexContent/xs:restriction]" />
	<xsl:variable name="linkName">
	    <xsl:call-template name="type_to_class">
	        <xsl:with-param name="type" select="@name"/>
	    </xsl:call-template>
	</xsl:variable>
    
    <!-- Handle associations common to links and nodes -->
    <xsl:call-template name="network_pmtv_subelements">
        <xsl:with-param name="schemaName" select="$schemaName"/>
        <xsl:with-param name="baseUri" select="$baseUri"/>
        <xsl:with-param name="baseClass" select="$baseClass"/>
        <xsl:with-param name="restrictedClass" select="$restrictedClass"/>
    </xsl:call-template>
    
    <!-- Handle link to start node and end node associations -->
    <xsl:if test="$restrictedClass">
        <!-- directed node member of restricted type points to nodes -->
        <xsl:variable name="directedNodeMember" select="concat($schemaName,':DirectedNode_',$linkName)"/>
        <xsl:for-each select="$restrictedClass/xs:complexContent/xs:restriction/xs:sequence/xs:element[@ref = $directedNodeMember]">
            <xsl:call-template name="network_pmtv_restricted">
                <xsl:with-param name="referencingElement" select="$referencingElement"/>
                <xsl:with-param name="wrapperElement">StartNode</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="network_pmtv_restricted">
                <xsl:with-param name="referencingElement" select="$referencingElement"/>
                <xsl:with-param name="wrapperElement">EndNode</xsl:with-param>
            </xsl:call-template>
        </xsl:for-each>
    </xsl:if>
</xsl:template>

<!-- Writes the association sub-elements specific to a node network feature class -->
<xsl:template name="node_subelements">
    <xsl:param name="schemaName"/>
    <xsl:param name="baseUri"/>
    <xsl:param name="baseClass"/>

	<!-- Association's are referenced by the link's restricted base class -->
	<xsl:variable name="restrictedClass" select="key('schemaKey',$baseUri)/xs:complexType[(@name = $baseClass) and xs:complexContent/xs:restriction]" />
	<xsl:variable name="nodeName">
	    <xsl:call-template name="type_to_class">
	        <xsl:with-param name="type" select="@name"/>
	    </xsl:call-template>
	</xsl:variable>

    <!-- Handle associations common to links and nodes -->
    <xsl:call-template name="network_pmtv_subelements">
        <xsl:with-param name="schemaName" select="$schemaName"/>
        <xsl:with-param name="baseUri" select="$baseUri"/>
        <xsl:with-param name="baseClass" select="$baseClass"/>
        <xsl:with-param name="restrictedClass" select="$restrictedClass"/>
    </xsl:call-template>
    
    <!-- Handle node to layer association -->
	<xsl:variable name="schemaUri" select="namespace::node()[local-name() = $schemaName]" />
	<xsl:variable name="elemName" select="concat('NodeLayer_',$nodeName)" />
	<!-- Find the global element that references this node class's layer -->
	<xsl:variable name="globalElements" select="key('schemaKey',$schemaUri)/xs:element[@name=$elemName]" />
	<xsl:for-each select="$globalElements[position() = 1]">
	    <!-- This global element's substitution group references the global element for the layer's primitives -->
	    <xsl:variable name="fullName">
		    <xsl:call-template name="get_fullname" >
			    <xsl:with-param name="typeName" select="@substitutionGroup" />
		    </xsl:call-template>			
	    </xsl:variable>
        <!-- decode layer schema and name from the its primitives element -->
	    <xsl:variable name="localName" select="substring-before(substring-after($fullName,' '),' ')"/>
	    <xsl:variable name="nameSpace" select="substring-after(substring-after($fullName,' '),' ')"/>
        <xsl:variable name="className" select="substring-before($localName,'_Primitive')"/>
        <!-- Create the node to layer association property -->
        <xsl:element name="AssociationProperty" >
            <xsl:attribute name="name">
                <xsl:choose>
                    <xsl:when test="@fdo:assocPropName">
                        <xsl:value-of select="@fdo:assocPropName"/>
                    </xsl:when>
                    <xsl:when test="@fdov2:assocPropName">
                        <xsl:value-of select="@fdov2:assocPropName"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="'Layer'"/>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:attribute>
            <xsl:attribute name="associatedClassSchema"><xsl:value-of select="$nameSpace"/></xsl:attribute>
            <xsl:attribute name="associatedClass"><xsl:value-of select="$className"/></xsl:attribute>
            <xsl:call-template name="association_attributes"/>
        </xsl:element>
	</xsl:for-each>
</xsl:template>

<!-- Writes the association sub-elements for network feature classes (nodes and links) -->
<xsl:template name="network_pmtv_subelements">
    <xsl:param name="schemaName"/>
    <xsl:param name="baseUri"/>
    <xsl:param name="baseClass"/>
    <xsl:param name="restrictedClass"/>

	<xsl:variable name="referencingElement">
	    <xsl:call-template name="type_to_class">
	        <xsl:with-param name="type" select="@name" />
	    </xsl:call-template>
	</xsl:variable>
	<xsl:variable name="schemaUri" select="namespace::node()[local-name() = $schemaName]" />
	<xsl:variable name="typeName" select="concat($schemaName,':',@name)" />

    <!-- handle primitive to network association -->

	<!-- Find the global element in the current schema, whose type is the current complexType -->
	<xsl:variable name="globalElements" select="key('schemaKey',$schemaUri)/xs:element[@type=$typeName]" />
	<xsl:for-each select="$globalElements[position() = 1]">
	    <!-- The first element's substitution group is the global element for the network's primitives -->
	    <xsl:variable name="fullName">
		    <xsl:call-template name="get_fullname" >
			    <xsl:with-param name="typeName" select="@substitutionGroup" />
		    </xsl:call-template>			
	    </xsl:variable>
	    <!-- decode network schema and name from this global element -->
	    <xsl:variable name="localName" select="substring-before(substring-after($fullName,' '),' ')"/>
	    <xsl:variable name="nameSpace" select="substring-after(substring-after($fullName,' '),' ')"/>
        <xsl:variable name="className" select="substring-before($localName,'_Primitive')"/>
        <xsl:if test="not(string-length($className) = 0)">
            <!-- write the primitive to network association property -->
            <xsl:element name="NetworkProperty">
                <xsl:element name="AssociationProperty" >
                    <xsl:attribute name="name">
                        <xsl:choose>
                            <xsl:when test="@fdo:assocPropName">
                                <xsl:value-of select="@fdo:assocPropName"/>
                            </xsl:when>
                            <xsl:when test="@fdov2:assocPropName">
                                <xsl:value-of select="@fdov2:assocPropName"/>
                            </xsl:when>
                            <xsl:otherwise>
                                <xsl:value-of select="'Network'"/>
                            </xsl:otherwise>
                        </xsl:choose>
                    </xsl:attribute>
                    <xsl:attribute name="associatedClassSchema"><xsl:value-of select="$nameSpace"/></xsl:attribute>
                    <xsl:attribute name="associatedClass"><xsl:value-of select="$className"/></xsl:attribute>
                    <xsl:call-template name="association_attributes"/>
                </xsl:element>
            </xsl:element>
        </xsl:if>
	</xsl:for-each>
	<!-- If this class has a restricted class, generated feature reference and parent primitive
	     associations from the restricted class.
	-->
    <xsl:if test="$restrictedClass">
        <xsl:variable name="featMember" select="concat($schemaName,':ReferenceFeature_',$referencingElement)"/>
        <xsl:for-each select="$restrictedClass/xs:complexContent/xs:restriction/xs:sequence/xs:element[@ref = $featMember]">
            <xsl:call-template name="network_pmtv_restricted">
                <xsl:with-param name="referencingElement" select="$referencingElement"/>
                <xsl:with-param name="wrapperElement">ReferenceFeatureProperty</xsl:with-param>
            </xsl:call-template>
        </xsl:for-each>
        <xsl:variable name="parentMember" select="concat($schemaName,':ParentPrimitive_',$referencingElement)"/>
        <xsl:for-each select="$restrictedClass/xs:complexContent/xs:restriction/xs:sequence/xs:element[@ref = $parentMember]">
            <xsl:call-template name="network_pmtv_restricted">
                <xsl:with-param name="referencingElement" select="$referencingElement"/>
                <xsl:with-param name="wrapperElement">ParentNetworkProperty</xsl:with-param>
            </xsl:call-template>
        </xsl:for-each>
    </xsl:if>
</xsl:template>

<!-- This template acts on a member property sub-element of a restricted complexType. It finds
     the class pointed, by this member property sub-element, and generates an Association property.
-->
<xsl:template name="network_pmtv_restricted">
    <xsl:param name="referencingElement"/>
    <xsl:param name="wrapperElement"/>
    <!-- skip sub-element if not allowed to have instances -->
    <xsl:if test="not(@maxOccurs = 0)">
        <xsl:element name="{$wrapperElement}">
            <xsl:variable name="fullName">
				<xsl:call-template name="get_fullname" >
    				<xsl:with-param name="typeName" select="@ref" />
	    	    </xsl:call-template>			
		    </xsl:variable>
			<xsl:variable name="uriName"><xsl:value-of select="substring-before($fullName,' ')"/></xsl:variable>
		    <xsl:variable name="localName" select="substring-before(substring-after($fullName,' '),' ')"/>
        	<!-- Find the member property global element referenced by the above sub-element -->
        	<xsl:variable name="memberElement" select="key('schemaKey',$uriName)/xs:element[(@name = $localName)]" />
            <xsl:if test="$memberElement">
                <xsl:variable name="fullType">
				    <xsl:call-template name="get_fullname" >
    				    <xsl:with-param name="typeName" select="$memberElement/@type" />
	    	        </xsl:call-template>			
		        </xsl:variable>
    			<xsl:variable name="uriType"><xsl:value-of select="substring-before($fullType,' ')"/></xsl:variable>
        	    <xsl:variable name="schemaType" select="substring-after(substring-after($fullType,' '),' ')"/>
		        <xsl:variable name="localType" select="substring-before(substring-after($fullType,' '),' ')"/>
        	    <xsl:variable name="memberType" select="key('schemaKey',$uriType)/xs:complexType[(@name = $localType)]" />
                <!-- Find the type for the member property global element -->
                <xsl:for-each select="$memberType">
                    <xsl:call-template name="network_pmtv_subelement">
                        <xsl:with-param name="referencingElement" select="$referencingElement"/>
                        <xsl:with-param name="prefix" select="$wrapperElement"/>
                        <xsl:with-param name="schemaName" select="$schemaType"/>
                        <xsl:with-param name="className" select="$localType"/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:if>
        </xsl:element>
    </xsl:if>
</xsl:template>

<!-- This template acts on a member property type. It finds the type of element referenced
     by this property type and generates an association property.
-->
<xsl:template name="network_pmtv_subelement">
    <xsl:param name="referencingElement"/>
    <xsl:param name="prefix"/>
    <xsl:param name="schemaName" />
    <xsl:param name="className"/>
    
    <!-- If the member property type is prefixed by the referencing complexType,
         strip out the referencer name.
    -->
    <xsl:variable name="propPrefix" select="concat($referencingElement,'_')"/>
    <xsl:variable name="propName">
        <xsl:choose>
            <xsl:when test="starts-with(@name,$propPrefix)">
                <xsl:value-of select="substring-after(@name,$propPrefix)"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="@name"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>
    <!-- Get the name of the element being referenced by this member property -->
    <xsl:variable name="fullName">
        <xsl:call-template name="get_fullname" >
    	    <xsl:with-param name="typeName" select="xs:complexContent/xs:restriction/xs:sequence/xs:element[position() = 1]/@ref" />
	    </xsl:call-template>			
    </xsl:variable>
	<xsl:variable name="refElementUri"><xsl:value-of select="substring-before($fullName,' ')"/></xsl:variable>
    <xsl:variable name="refElementName" select="substring-before(substring-after($fullName,' '),' ')"/>
    <xsl:variable name="refElementSchema" select="substring-after(substring-after($fullName,' '),' ')"/>
   	<!-- Get the element being referenced -->
   	<xsl:variable name="refElement" select="key('schemaKey',$refElementUri)/xs:element[(@name = $refElementName)]" />
    <xsl:choose>
        <xsl:when test="not($refElement) or $refElement/@type">
            <xsl:variable name="fullType">
                <xsl:choose>
                    <xsl:when test="$refElement">
                        <!-- Referenced element, and it has a type. Set referenced class 
                             from this type.
                        -->
                        <xsl:call-template name="get_fullname" >
                            <xsl:with-param name="typeName" select="$refElement/@type" />
                        </xsl:call-template>			
                    </xsl:when>
                    <xsl:otherwise>
                            <!-- Referenced element not found. Make best assumption that class is
                                 named the same as the element.
                            -->
                        <xsl:value-of select="$fullName"/>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:variable>
            <xsl:variable name="typeName" select="substring-before(substring-after($fullType,' '),' ')"/>
            <xsl:variable name="typeSchema" select="substring-after(substring-after($fullType,' '),' ')"/>
            <xsl:if test="$typeName">
                <!-- Have a type so generate an association to it -->
                <xsl:element name="AssociationProperty" >
                    <xsl:attribute name="name"><xsl:value-of select="$propName"/></xsl:attribute>
                    <xsl:attribute name="associatedClassSchema"><xsl:value-of select="$typeSchema"/></xsl:attribute>
                    <xsl:attribute name="associatedClass">
                        <xsl:choose>
                            <xsl:when test="$refElement">
                                <xsl:call-template name="type_to_class" >
                                    <xsl:with-param name="type" select="$typeName" />
                                </xsl:call-template>			
                            </xsl:when>
                            <xsl:otherwise>
                                <xsl:value-of select="$typeName"/>
                            </xsl:otherwise>
                        </xsl:choose>
                    </xsl:attribute>
                    <xsl:call-template name="association_attributes"/>
	            </xsl:element>
            </xsl:if>
        </xsl:when>
        <xsl:otherwise>
            <!-- Special case. When referenced element found but it has no type, assume that this
                 is a link to node reference when start and end nodes are of different type.
                 The start and end nodes have global elements with substitution group set to
                 referenced element.
            -->
            <!-- Find start or end node global element ( start or end determined by prefix ) -->
          	<xsl:variable name="subElement" select="//xs:schema/xs:element[(@substitutionGroup = concat($refElementSchema,':',$refElementName)) and ((@fdo:refType=$prefix) or (@fdov2:refType=$prefix))]" />
            <xsl:for-each select="$subElement">
                <!-- association property name is global element name with link node class name
                     prefix stripped out.
                -->
                <xsl:variable name="subPropName">
                    <xsl:choose>
                        <xsl:when test="starts-with($subElement/@name,$propPrefix)">
                            <xsl:value-of select="substring-after($subElement/@name,$propPrefix)"/>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="$subElement/@name"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:variable>
                <!-- Get assocated node class name from global element's type -->
                <xsl:variable name="subFullType">
	                <xsl:call-template name="get_fullname" >
		                <xsl:with-param name="typeName" select="$subElement/@type" />
                    </xsl:call-template>			
                </xsl:variable>
                <xsl:variable name="subTypeName" select="substring-before(substring-after($subFullType,' '),' ')"/>
                <xsl:variable name="subTypeSchema" select="substring-after(substring-after($subFullType,' '),' ')"/>
                <xsl:if test="$subTypeName">
                    <!-- Generate the association property -->
                    <xsl:element name="AssociationProperty" >
                        <xsl:attribute name="name"><xsl:value-of select="$subPropName"/></xsl:attribute>
                        <xsl:attribute name="associatedClassSchema"><xsl:value-of select="$subTypeSchema"/></xsl:attribute>
                        <xsl:attribute name="associatedClass">
                            <xsl:call-template name="type_to_class">
                                <xsl:with-param name="type" select="$subTypeName"/>
                            </xsl:call-template>
                        </xsl:attribute>
                        <xsl:call-template name="association_attributes"/>
	                </xsl:element>
                </xsl:if>
            </xsl:for-each>                
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<!-- Writes the generic sub-elements (description and SAD) for an FDO element -->
<xsl:template name="element_subelements">
    <xsl:param name="schemaName"/>
	<xsl:param name="lastAnno" select="'false'"/>
	<xsl:for-each select="xs:annotation" >
		<xsl:call-template name="annotation">
		    <xsl:with-param name="schemaName" select="$schemaName" />
		    <xsl:with-param name="lastAnno" select="$lastAnno" />
		</xsl:call-template>
	</xsl:for-each>
</xsl:template>

<xsl:template name="annotation">
    <xsl:param name="schemaName" />
    <xsl:param name="lastAnno" />
	<xsl:choose>
		<xsl:when test="$lastAnno = 'false'">
			<xsl:for-each select="xs:documentation[position() = 1]" >
				<xsl:call-template name="documentation">
	                <xsl:with-param name="schemaName" select="$schemaName" />
				</xsl:call-template>
			</xsl:for-each>
			<xsl:for-each select="xs:appinfo[position() = 1]" >
				<xsl:call-template name="appinfo">
	                <xsl:with-param name="schemaName" select="$schemaName" />
				</xsl:call-template>
			</xsl:for-each>
		</xsl:when>
		<xsl:otherwise>
			<xsl:for-each select="xs:documentation[(position() = last()) and not (position() = 1) ]" >
				<xsl:call-template name="documentation">
	                <xsl:with-param name="schemaName" select="$schemaName" />
				</xsl:call-template>
			</xsl:for-each>
			<xsl:for-each select="xs:appinfo[(position() = last()) and not (position() = 1)]" >
				<xsl:call-template name="appinfo">
	                <xsl:with-param name="schemaName" select="$schemaName" />
				</xsl:call-template>
			</xsl:for-each>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- Extracts schema element descriptions from documentation -->
<xsl:template name="documentation">
    <xsl:param name="schemaName" />
	<xsl:element name="Description">
		<xsl:value-of select="." />
	</xsl:element>
	<xsl:call-template name="documentation_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
</xsl:template>

<!-- Extracts Schema Attribute Dictionaries from appinfo -->
<xsl:template name="appinfo">
    <xsl:param name="schemaName"/>
	<xsl:element name="SAD">
		<xsl:for-each select="fdo:attribute" >
			<xsl:element name="SADItem">
				<xsl:attribute name="name" >
					<xsl:value-of select="@name" />
				</xsl:attribute>
				<xsl:value-of select="." />
			</xsl:element>
		</xsl:for-each>
		<xsl:for-each select="fdov2:attribute" >
			<xsl:element name="SADItem">
				<xsl:attribute name="name" >
					<xsl:value-of select="@name" />
				</xsl:attribute>
				<xsl:value-of select="." />
			</xsl:element>
		</xsl:for-each>
	</xsl:element>
	<xsl:call-template name="appinfo_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
</xsl:template>

<!-- Converts type name to class name. Strips off "Type" suffix from name if present.-->
<xsl:template name="type_to_class">
    <xsl:param name="type"/>
    <xsl:choose>
      <!-- Strip off "Type" suffix if type name ends in "_Type" but is not "_Type"-->
      <xsl:when test="(string-length($type) &gt; 5) and (substring($type,string-length($type)-4) = '_Type')">
        <xsl:value-of select="substring($type,1,string-length($type)-5)"/>
      </xsl:when>
      <!-- Strip off "Type" suffix if type name ends in "Type" but is not "Type"-->
      <xsl:when test="(string-length($type) &gt; 4) and (substring($type,string-length($type)-3) = 'Type')">
        <xsl:value-of select="substring($type,1,string-length($type)-4)"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$type"/>
      </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<!-- The following templates log errors for unsupported GML elements and attributes -->
<xsl:template name="schema_errors" >
	<xsl:param name="schemaName" />
	<xsl:if test="not(@attributeFormDefault = 'unqualified')">
		<xsl:call-template name="unsupported_attribute_value" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >attributeFormDefault</xsl:with-param>
			<xsl:with-param name="attributeValue" select="@attributeFormDefault" />
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="not(@elementFormDefault = 'qualified')">
		<xsl:call-template name="unsupported_attribute_value" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >elementFormDefault</xsl:with-param>
			<xsl:with-param name="attributeValue" select="@elementFormDefault" />
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="@blockDefault">
		<xsl:call-template name="unsupported_attribute" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >blockDefault</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="@finalDefault">
		<xsl:call-template name="unsupported_attribute" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >finalDefault</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="xs:include">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >include</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="xs:import">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >import</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="xs:simpleType">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >simpleType</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="xs:group">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >group</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="xs:attributeGroup">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >attributeGroup</xsl:with-param>
			<xsl:with-param name="errorLevel" >l</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="xs:attribute">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >attribute</xsl:with-param>
			<xsl:with-param name="errorLevel" >l</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="xs:redefine">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >redefine</xsl:with-param>
			<xsl:with-param name="errorLevel" >n</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="xs:notation">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >notation</xsl:with-param>
			<xsl:with-param name="errorLevel" >n</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:call-template name="general_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
</xsl:template>

<xsl:template name="element_global_errors" >
	<xsl:param name="schemaName"/>
	<xsl:param name="autogenElem"/>
	<xsl:param name="typeUri"/>
	<xsl:param name="typeName"/>
	<!-- Global elements are round-tripped only of their names match their types or they
	 represent associations to classes outside the current schema. When error level is 
	 high, log an error for other global elements, which won't be round-tripped.
	-->
	<xsl:if test="$autogenElem='no'" >
	        <!-- Check if element and element type names match -->

            <!-- Element and type must be in same schema -->
	        <xsl:if test="not(ancestor::xs:schema/@targetNamespace = $typeUri)" >
                <!-- Names failed strict match, log a high level error -->
                <xsl:element name="Error">
		            <xsl:attribute name="number">1058</xsl:attribute>
		            <xsl:attribute name="level">l</xsl:attribute>
		            <fdo:P><xsl:value-of select="$schemaName" /></fdo:P>
		            <fdo:P><xsl:call-template name="bld_context_path" /></fdo:P>
		            <fdo:P><xsl:value-of select="$typeUri" /></fdo:P>
	            </xsl:element>
	        </xsl:if>
	        
	        <!-- First check strict match ( typeName = elementName + "Type" ).-->
	        <xsl:if test="not($typeName) or not(concat(@name,'Type')=$typeName)" >
                <!-- Names failed strict match, log a high level error -->
                <xsl:element name="Error">
		            <xsl:attribute name="number">1056</xsl:attribute>
		            <xsl:attribute name="level">h</xsl:attribute>
		            <fdo:P><xsl:value-of select="$schemaName" /></fdo:P>
		            <fdo:P><xsl:call-template name="bld_context_path" /></fdo:P>
		            <fdo:P><xsl:value-of select="concat(@name,'Type')" /></fdo:P>
	            </xsl:element>
	        </xsl:if>
            <!-- For lower error levels, type name does not have to have 'Type' suffix.
                 Do the non-strict match check ( (typeName = elementName + "Type") or (typeName = elementName))
             -->
	        <xsl:if test="not($typeName) or (not(@name=$typeName) and not(concat(@name,'Type')=$typeName))" >
                <!-- Element and type names completely different, log a 
                     error since the non-strict name match failed.
                     Caller is responsible for discarding error 1056 and error 1023 is logged.
                -->
                <xsl:element name="Error">
	                <xsl:attribute name="number">1023</xsl:attribute>
	                <xsl:attribute name="level">l</xsl:attribute>
	                <fdo:P><xsl:value-of select="$schemaName" /></fdo:P>
	                <fdo:P><xsl:call-template name="bld_context_path" /></fdo:P>
	                <fdo:P><xsl:value-of select="@name" /></fdo:P>
	                <fdo:P><xsl:value-of select="concat(@name,'Type')" /></fdo:P>
                </xsl:element>
            </xsl:if>	
	</xsl:if>
	<xsl:if test="@default">
		<xsl:call-template name="unsupported_attribute" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >default</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:for-each select="xs:key|xs:unique">
	    <xsl:if test="xs:annotation">
		    <xsl:call-template name="unsupported_element" >
			    <xsl:with-param name="schemaName" >
				    <xsl:value-of select="$schemaName" />
			    </xsl:with-param>
			    <xsl:with-param name="elementName" >annotation</xsl:with-param>
			    <xsl:with-param name="errorLevel" >h</xsl:with-param>
		    </xsl:call-template>
	    </xsl:if>
	    <xsl:for-each select="xs:field">
	        <xsl:if test="xs:annotation">
		        <xsl:call-template name="unsupported_element" >
			        <xsl:with-param name="schemaName" >
				        <xsl:value-of select="$schemaName" />
			        </xsl:with-param>
			        <xsl:with-param name="elementName" >annotation</xsl:with-param>
			        <xsl:with-param name="errorLevel" >h</xsl:with-param>
		        </xsl:call-template>
	        </xsl:if>
	    </xsl:for-each>
	</xsl:for-each>
	<xsl:call-template name="element_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
</xsl:template>

<xsl:template name="element_ref_errors">
	<xsl:param name="schemaName" />
	<xsl:param name="typeAbstract" />
	<xsl:param name="typeElem"/>
	<xsl:variable name="elemAbstract">
		<xsl:choose>
			<xsl:when test="@abstract"><xsl:value-of select="@abstract" /></xsl:when>
			<xsl:otherwise>false</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:if test="not($typeAbstract = $elemAbstract)">
		<xsl:variable name="typeContext" >
			<xsl:for-each select="$typeElem">
				<xsl:call-template name="bld_context_path" />
			</xsl:for-each>
		</xsl:variable>
		<xsl:variable name="elemContext" >
			<xsl:call-template name="bld_context_path" />
		</xsl:variable>
		<xsl:element name="Error">
			<xsl:attribute name="number">1036</xsl:attribute>
			<xsl:attribute name="level">h</xsl:attribute>
			<fdo:P><xsl:value-of select="$schemaName" /></fdo:P>
			<fdo:P><xsl:value-of select="$typeContext" /></fdo:P>
			<fdo:P><xsl:value-of select="$elemContext" /></fdo:P>
		</xsl:element>
	</xsl:if>
</xsl:template>

<xsl:template name="complexType_errors" >
	<xsl:param name="schemaName" />
    <xsl:param name="classType"/>
    <xsl:param name="className"/>
	<xsl:if test="@mixed = 'true'">
		<xsl:call-template name="unsupported_attribute_value" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >mixed</xsl:with-param>
			<xsl:with-param name="attributeValue" select="@mixed" />
			<xsl:with-param name="errorLevel" >l</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="@block">
		<xsl:call-template name="unsupported_attribute" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >block</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="@final">
		<xsl:call-template name="unsupported_attribute" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >final</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="xs:simpleContent">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >simpleContent</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<!-- Network type classes will have restrictions -->
	<xsl:if test="complexContent/xs:restriction and not(starts-with($classType,'Network'))">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >restriction</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="$className = @name">
	    <!-- if class name is the same as the complexType name then 
	         there was no 'Type' suffix stripped from the complexType name.
	         At the high error level, all complexType names must end with
	         'Type'.
	    -->
	    <xsl:variable name="context" >
		    <xsl:call-template name="bld_context_path" />
	    </xsl:variable>
	    <xsl:element name="Error">
		    <xsl:attribute name="number">1057</xsl:attribute>
		    <xsl:attribute name="level">h</xsl:attribute>
		    <fdo:P><xsl:value-of select="$schemaName" /></fdo:P>
		    <fdo:P><xsl:value-of select="$context" /></fdo:P>
	    </xsl:element>
	</xsl:if>
	<xsl:call-template name="member_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
</xsl:template>

<xsl:template name="simpleType_errors" >
	<xsl:param name="schemaName" />
  <xsl:if test="xs:union">
    <xsl:call-template name="unsupported_element" >
      <xsl:with-param name="schemaName" >
        <xsl:value-of select="$schemaName" />
      </xsl:with-param>
      <xsl:with-param name="elementName" >union</xsl:with-param>
      <xsl:with-param name="errorLevel" >l</xsl:with-param>
    </xsl:call-template>
  </xsl:if>
  <xsl:if test="$error_level='h'">
    <xsl:if test="@final">
		  <xsl:call-template name="unsupported_attribute" >
			  <xsl:with-param name="schemaName" >
				  <xsl:value-of select="$schemaName" />
			  </xsl:with-param>
			  <xsl:with-param name="attributeName" >final</xsl:with-param>
			  <xsl:with-param name="errorLevel" >h</xsl:with-param>
		  </xsl:call-template>
	  </xsl:if>
	  <xsl:if test="xs:annotation">
		  <xsl:call-template name="unsupported_element" >
			  <xsl:with-param name="schemaName" >
				  <xsl:value-of select="$schemaName" />
			  </xsl:with-param>
			  <xsl:with-param name="elementName" >annotation</xsl:with-param>
			  <xsl:with-param name="errorLevel" >h</xsl:with-param>
		  </xsl:call-template>
	  </xsl:if>
	  <xsl:if test="xs:list">
		  <xsl:call-template name="unsupported_element" >
			  <xsl:with-param name="schemaName" >
				  <xsl:value-of select="$schemaName" />
			  </xsl:with-param>
			  <xsl:with-param name="elementName" >list</xsl:with-param>
			  <xsl:with-param name="errorLevel" >h</xsl:with-param>
		  </xsl:call-template>
	  </xsl:if>
	  <xsl:for-each select="xs:restriction">
	      <xsl:call-template name="restriction_errors">
		      <xsl:with-param name="schemaName" select="$schemaName"/>
	      </xsl:call-template>
	  </xsl:for-each>
  </xsl:if>
</xsl:template>

<xsl:template name="restriction_errors" >
	<xsl:param name="schemaName" />
	<xsl:if test="xs:annotation">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >annotation</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
  
	<!-- Log error for any restriction facets not supported -->
	<xsl:for-each select="*">
		<xsl:variable name="fullName">
			<xsl:call-template name="get_fullname" >
				<xsl:with-param name="typeName" select="name()" />
			</xsl:call-template>			
		</xsl:variable>
		<xsl:variable name="uriName"><xsl:value-of select="substring-before($fullName,' ')"/></xsl:variable>
		<xsl:variable name="localName"><xsl:value-of select="substring-before(substring-after($fullName,' '),' ')"/></xsl:variable>
	    <xsl:if test="not($uriName='') and not($localName='annotation' or $localName='enumeration' or $localName='totalDigits' or $localName='fractionDigits' or $localName='maxLength' or $localName='maxInclusive' or $localName='minExclusive' or $localName='maxExclusive' or $localName='minInclusive')">
	        <xsl:for-each select="..">
		        <xsl:call-template name="unsupported_element" >
			        <xsl:with-param name="schemaName" >
				        <xsl:value-of select="$schemaName" />
			        </xsl:with-param>
			        <xsl:with-param name="elementName" select="$localName"/>
			        <xsl:with-param name="errorLevel" >h</xsl:with-param>
		        </xsl:call-template>
	        </xsl:for-each>
	    </xsl:if>
	</xsl:for-each>
</xsl:template>

<xsl:template name="complexContent_errors" >
	<xsl:param name="schemaName" />
	<xsl:if test="@mixed = 'true'">
		<xsl:call-template name="unsupported_attribute_value" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >mixed</xsl:with-param>
			<xsl:with-param name="attributeValue" select="@mixed" />
			<xsl:with-param name="errorLevel" >l</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="xs:annotation">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >annotation</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:call-template name="general_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
</xsl:template>

<xsl:template name="extension_errors" >
	<xsl:param name="schemaName" />
	<xsl:if test="xs:annotation">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >annotation</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:call-template name="member_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
</xsl:template>

<xsl:template name="sequence_etc_errors" >
	<xsl:param name="schemaName" />
	<xsl:if test="xs:annotation">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >annotation</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:call-template name="member_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
</xsl:template>

<xsl:template name="member_errors" >
	<xsl:param name="schemaName" />
	<xsl:if test="xs:group">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >group</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="xs:all">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >all</xsl:with-param>
			<xsl:with-param name="errorLevel" >l</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="xs:choice">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >choice</xsl:with-param>
			<xsl:with-param name="errorLevel" >n</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="xs:attributeGroup">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >attributeGroup</xsl:with-param>
			<xsl:with-param name="errorLevel" >l</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="xs:attribute">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >attribute</xsl:with-param>
			<xsl:with-param name="errorLevel" >l</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="xs:anyAttribute">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >anyAttribute</xsl:with-param>
			<xsl:with-param name="errorLevel" >l</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:call-template name="general_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
</xsl:template>

<xsl:template name="element_nonglobal_errors" >
	<xsl:param name="schemaName" />
	<xsl:if test="@abstract and @abstract='true'">
		<xsl:call-template name="unsupported_attribute_value" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >abstract</xsl:with-param>
			<xsl:with-param name="attributeValue" >true</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
  <xsl:if test="xs:complexType and not(xs:keyref or xs:complexType/xs:complexContent/xs:restriction[@base='gml:FeaturePropertyType'] or xs:complexType/xs:complexContent/xs:restriction[@base='gml:FeatureAssociationType'] or xs:complexType/xs:sequence/xs:element[@ref='gml:_Geometry'])">
    <xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName" >complexType</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:for-each select="xs:keyref">
	    <xsl:call-template name="keyref_errors">
		    <xsl:with-param name="schemaName" select="$schemaName"/>
	    </xsl:call-template>
	</xsl:for-each>
	<xsl:call-template name="element_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
</xsl:template>

<xsl:template name="element_errors" >
	<xsl:param name="schemaName" />
	<xsl:if test="@block">
		<xsl:call-template name="unsupported_attribute" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >block</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="@final">
		<xsl:call-template name="unsupported_attribute" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >final</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="@fixed">
		<xsl:call-template name="unsupported_attribute" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >fixed</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="@form">
		<xsl:call-template name="unsupported_attribute" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >form</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:if test="@nillable">
		<xsl:call-template name="unsupported_attribute" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >nillable</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:call-template name="general_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
</xsl:template>

<xsl:template name="keyref_errors" >
	<xsl:param name="schemaName" />
	<xsl:if test="xs:annotation">
		<xsl:call-template name="unsupported_element" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="elementName">annotation</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:for-each select="xs:selector">
	    <xsl:if test="xs:annotation">
		    <xsl:call-template name="unsupported_element" >
			    <xsl:with-param name="schemaName" >
				    <xsl:value-of select="$schemaName" />
			    </xsl:with-param>
			    <xsl:with-param name="elementName">annotation</xsl:with-param>
			    <xsl:with-param name="errorLevel" >h</xsl:with-param>
		    </xsl:call-template>
	    </xsl:if>
	    <xsl:for-each select="xs:field">
	        <xsl:if test="xs:annotation">
		        <xsl:call-template name="unsupported_element" >
			        <xsl:with-param name="schemaName" >
				        <xsl:value-of select="$schemaName" />
			        </xsl:with-param>
			        <xsl:with-param name="elementName">annotation</xsl:with-param>
			        <xsl:with-param name="errorLevel" >h</xsl:with-param>
		        </xsl:call-template>
	        </xsl:if>
	    </xsl:for-each>
	</xsl:for-each>
	<xsl:call-template name="general_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
</xsl:template>

<xsl:template name="documentation_errors" >
	<xsl:param name="schemaName" />
	<xsl:if test="@source">
		<xsl:call-template name="unsupported_attribute" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >source</xsl:with-param>
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:call-template name="general_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
</xsl:template>

<xsl:template name="appinfo_errors" >
	<xsl:param name="schemaName" />
	<xsl:if test="@source and not(@source=$gFdoUri or @source=$gFdoV2Uri)">
		<xsl:call-template name="unsupported_attribute_value" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >source</xsl:with-param>
			<xsl:with-param name="attributeValue" select="@source" />
			<xsl:with-param name="errorLevel" >h</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
	<xsl:for-each select="*[not(name()='fdo:attribute') and not(name()='fdo:DefaultDataModel') and not(name()='fdov2:attribute') and not(name()='fdov2:DefaultDataModel')]">
		<xsl:variable name="subElementName" select="local-name()"/>
		<xsl:for-each select="..">
		    <xsl:call-template name="unsupported_element" >
			    <xsl:with-param name="schemaName" >
				    <xsl:value-of select="$schemaName" />
			    </xsl:with-param>
			    <xsl:with-param name="elementName" >
			        <xsl:value-of select="$subElementName"/>
			    </xsl:with-param>
			    <xsl:with-param name="errorLevel" >h</xsl:with-param>
		    </xsl:call-template>
		</xsl:for-each>
	</xsl:for-each>
	<xsl:call-template name="general_errors">
		<xsl:with-param name="schemaName" select="$schemaName"/>
	</xsl:call-template>
</xsl:template>

<xsl:template name="general_errors" >
	<xsl:param name="schemaName"/>
	<xsl:if test="@maxOccurs and not(@maxOccurs='0' or @maxOccurs='1' or @maxOccurs='unbounded')">
		<xsl:call-template name="unsupported_attribute_value" >
			<xsl:with-param name="schemaName" >
				<xsl:value-of select="$schemaName" />
			</xsl:with-param>
			<xsl:with-param name="attributeName" >maxOccurs</xsl:with-param>
			<xsl:with-param name="attributeValue" select="@maxOccurs" />
			<xsl:with-param name="errorLevel" >n</xsl:with-param>
		</xsl:call-template>
	</xsl:if>
  <xsl:if test="$error_level='h'">
    <xsl:if test="@id">
      <xsl:call-template name="unsupported_attribute" >
        <xsl:with-param name="schemaName" >
          <xsl:value-of select="$schemaName" />
        </xsl:with-param>
        <xsl:with-param name="attributeName" >id</xsl:with-param>
        <xsl:with-param name="errorLevel" >h</xsl:with-param>
      </xsl:call-template>
    </xsl:if>
    <!-- Log an error for each attribute that has a namespace qualifier but
	       the namespace is not FDO
	  -->
    <xsl:for-each select="@*[contains(name(),':')]">
      <xsl:variable name="attrName" select="name()"/>
      <xsl:for-each select="..">
        <xsl:variable name="attrUri" select="namespace::node()[local-name() = substring-before($attrName,':')]" />
        <xsl:if test="not($attrUri = $gFdoUri or $attrUri = $gFdoV2Uri)">
          <xsl:call-template name="unsupported_attribute" >
            <xsl:with-param name="schemaName" >
              <xsl:value-of select="$schemaName" />
            </xsl:with-param>
            <xsl:with-param name="attributeName" select="$attrName"/>
            <xsl:with-param name="errorLevel" >h</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:if>
</xsl:template>

<!-- Logs an error for a restriction facet that does not apply to its type -->
<xsl:template name="unsupported_restriction_type">
    <xsl:param name="schemaName"/>
    <xsl:param name="facetName"/>
    <xsl:for-each select="xs:restriction">
	    <xsl:variable name="contextRoot" >
		    <xsl:call-template name="bld_context_path" />
	    </xsl:variable>
	    <xsl:variable name="context" select="concat($contextRoot,'[base=',@base,']')"/>
	    <xsl:element name="Error">
		    <xsl:attribute name="number">1019</xsl:attribute>
		    <xsl:attribute name="level">h</xsl:attribute>
		    <fdo:P><xsl:value-of select="$schemaName" /></fdo:P>
		    <fdo:P><xsl:value-of select="$facetName" /></fdo:P>
		    <fdo:P><xsl:value-of select="$context" /></fdo:P>
	    </xsl:element>
    </xsl:for-each>
</xsl:template>

<!-- Logs an error for an unsupported element -->
<xsl:template name="unsupported_element" >
	<xsl:param name="schemaName" />
	<xsl:param name="elementName" />
	<xsl:param name="errorLevel" />
	<xsl:variable name="context" >
		<xsl:call-template name="bld_context_path" />
	</xsl:variable>
	<xsl:element name="Error">
		<xsl:attribute name="number">1019</xsl:attribute>
		<xsl:attribute name="level"><xsl:value-of select="$errorLevel" /></xsl:attribute>
		<fdo:P><xsl:value-of select="$schemaName" /></fdo:P>
		<fdo:P><xsl:value-of select="$elementName" /></fdo:P>
		<fdo:P><xsl:value-of select="$context" /></fdo:P>
	</xsl:element>
</xsl:template>

<!-- Logs an error for an unsupported attribute -->
<xsl:template name="unsupported_attribute" >
	<xsl:param name="schemaName" />
	<xsl:param name="attributeName" />
	<xsl:param name="errorLevel" />
	<xsl:variable name="context" >
		<xsl:call-template name="bld_context_path" />
	</xsl:variable>
	<xsl:element name="Error">
		<xsl:attribute name="number">1020</xsl:attribute>
		<xsl:attribute name="level"><xsl:value-of select="$errorLevel" /></xsl:attribute>
		<fdo:P><xsl:value-of select="$schemaName" /></fdo:P>
		<fdo:P><xsl:value-of select="$attributeName" /></fdo:P>
		<fdo:P><xsl:value-of select="$context" /></fdo:P>
	</xsl:element>
</xsl:template>

<!-- Logs an error for an attribute with an unsupported value -->
<xsl:template name="unsupported_attribute_value" >
	<xsl:param name="schemaName" />
	<xsl:param name="attributeName" />
	<xsl:param name="attributeValue" />
	<xsl:param name="errorLevel" />
	<xsl:variable name="context" >
		<xsl:call-template name="bld_context_path" />
	</xsl:variable>
	<xsl:element name="Error">
		<xsl:attribute name="number">1021</xsl:attribute>
		<xsl:attribute name="level"><xsl:value-of select="$errorLevel" /></xsl:attribute>
		<fdo:P><xsl:value-of select="$schemaName" /></fdo:P>
		<fdo:P><xsl:value-of select="$context" /></fdo:P>
		<fdo:P><xsl:value-of select="$attributeName" /></fdo:P>
		<fdo:P><xsl:value-of select="$attributeValue" /></fdo:P>
	</xsl:element>
</xsl:template>

<!-- Generates an xpath expression describing the location of the
 current element in the current document.
-->
<xsl:template name="bld_context_path" >
	<xsl:if test="not(local-name() = 'schema')" >
		<xsl:for-each select=".." >
			<xsl:call-template name="bld_context_path" />
		</xsl:for-each>
		<xsl:text>/</xsl:text>
	</xsl:if>
	<xsl:value-of select="name()" />
	<xsl:choose>
		<xsl:when test="@name" >
			<xsl:value-of select="concat('[@name=', concat(@name, ']'))" />
		</xsl:when>
		<xsl:when test="@ref" >
			<xsl:value-of select="concat('[@ref=', concat(@ref, ']'))" />
		</xsl:when>
	</xsl:choose>
</xsl:template>

</stylesheet>

  