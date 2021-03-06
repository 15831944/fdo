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
  Internal to GML Format. The Internal format is an XML format that 
  mirrors the FDO Feature Schema structure.  -->
<stylesheet version="1.0" 
 xmlns="http://www.w3.org/1999/XSL/Transform"
 xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
 xmlns:xs="http://www.w3.org/2001/XMLSchema"
 xmlns:fdo="http://fdo.osgeo.org/schemas"
 xmlns:gml="http://www.opengis.net/gml"
 xmlns:fds="http://fdo.osgeo.org/schemas/fds"
>
<!-- The URL prefix for the target namespace of each schema -->
<xsl:param name="customer_url" select="'fdo.osgeo.org/schemas/feature'"  />

<xsl:variable name="q_customer_url">
    <xsl:choose>
        <xsl:when test="$customer_url = ''">
        	<xsl:value-of select="'http://'" /> 
        </xsl:when>
        <xsl:otherwise>
        	<xsl:value-of select="concat('http://',$customer_url,'/')" /> 
        </xsl:otherwise>
    </xsl:choose>
</xsl:variable>

<!-- used for looking assocation property references -->
<xsl:key name="assocKey" match="AssociationProperty" use="concat(@associatedClassSchema,concat(':',@associatedClass))" />
<xsl:variable name="g_associationProperties" select="//AssociationProperty"/>

<!-- Determine whether using GML 2 or 3 feature property type. Assumes GML 2 if GML 3 type not defined -->
<xsl:variable name="g_featurePropertyType">
	<xsl:choose>
		<xsl:when test="//Schema/node()[concat(../@name,':',@name)='gml:FeaturePropertyType']">
			<xsl:value-of select="'gml:FeaturePropertyType'"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="'gml:FeatureAssociationType'"/>
		</xsl:otherwise>		
	</xsl:choose>
</xsl:variable>

<!-- Convert a Feature Schema to GML -->
<xsl:template match="Schema">
	<xsl:variable name="myUri" >
	    <xsl:call-template name="choose_targetNamespace">
            <xsl:with-param name="schema" select="@name"/>
            <xsl:with-param name="custom_targetNamespace" select="@targetNamespace"/>
	    </xsl:call-template>
	</xsl:variable>
	<xsl:element name="xs:schema" namespace="{$myUri}" >
		<!-- Write out the namespace and targetNamespace attributes -->
		<xsl:variable name="myns" >
		    <xsl:call-template name="schema_to_prefix">
                <xsl:with-param name="schema">
                    <xsl:call-template name="choose_schemaPrefix">
                        <xsl:with-param name="schema" select="@name" />
                        <xsl:with-param name="prefix" select="@prefix" />
                    </xsl:call-template>
                </xsl:with-param>
		    </xsl:call-template>
		</xsl:variable>
		<xsl:attribute name="targetNamespace" >
			<xsl:value-of select="$myUri" />
		</xsl:attribute>
		<xsl:attribute name="xmlns:fdo" >
			<xsl:value-of select="'http://fdo.osgeo.org/schemas'" />
		</xsl:attribute>
		<xsl:attribute name="xmlns:xs" >
			<xsl:value-of select="'http://www.w3.org/2001/XMLSchema'" />
		</xsl:attribute>
		<xsl:attribute name="xmlns:gml" >
			<xsl:value-of select="'http://www.opengis.net/gml'" />
		</xsl:attribute>
		<xsl:attribute name="{concat('xmlns:',$myns)}" >
			<xsl:value-of select="$myUri" />
		</xsl:attribute>
		<!-- Need a namespace declaration for each other schema referenced
		 via a base class reference, object property class or associated class
		 reference.
		-->
		<xsl:for-each select=".//node()[@baseSchema]" >
		    <xsl:call-template name="declare_namespace">
		        <xsl:with-param name="schema" select="@baseSchema"/>
		    </xsl:call-template>
		</xsl:for-each>
		<xsl:for-each select=".//node()[@classSchema]" >
		    <xsl:call-template name="declare_namespace">
		        <xsl:with-param name="schema" select="@classSchema"/>
		    </xsl:call-template>
		</xsl:for-each>
		<xsl:for-each select=".//node()[@associatedClassSchema]" >
		    <xsl:call-template name="declare_namespace">
		        <xsl:with-param name="schema" select="@associatedClassSchema"/>
		    </xsl:call-template>
		</xsl:for-each>
		<xsl:attribute name="elementFormDefault" >
			<xsl:value-of select="'qualified'" />
		</xsl:attribute>
		<xsl:attribute name="attributeFormDefault" >
			<xsl:value-of select="'unqualified'" />
		</xsl:attribute>
		<!-- Write the classes -->
		<xsl:call-template name="element_subelements" />
		<!-- Write the elements representing associated classes outside this
		 schema. These provide the link between elements representing associated
		 properties and their associated classes.
		-->
		<xsl:call-template name="external_elements" />
		<xsl:apply-templates select="*" />
	</xsl:element> 
</xsl:template>


  
<!-- Converts a feature class to GML -->
<xsl:template match="FeatureClass" >
	<!-- Create a global element if class has identity properties
	 this makes it a stand-alone class
	-->
	<xsl:if test="IdentityProperties or UniqueConstraints" >
		<xsl:call-template name="global_element" />
	</xsl:if>

  <!-- Write the class as a complex type -->
  <xsl:element name="xs:complexType">
    <xsl:call-template name="feature_class_attributes" />
    <xsl:call-template name="element_subelements" />
    
    <xsl:choose>
	<xsl:when test="@name='AbstractFeature'">
		<xsl:variable name="myUri" >
            <xsl:call-template name="choose_targetNamespace">
                <xsl:with-param name="schema" select="../@name"/>
                <xsl:with-param name="custom_targetNamespace" select="../@targetNamespace"/>
            </xsl:call-template>
		</xsl:variable>
		<xsl:choose>
			<xsl:when test="$myUri='http://www.opengis.net/gml'">
			  <!-- This is gml:AbstractFeature so don't extend from itself -->
			  <xsl:call-template name="FeatureClassNotExtended"/>
			</xsl:when>
			<xsl:otherwise>
			  <xsl:call-template name="FeatureClassExtended"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:call-template name="FeatureClassExtended"/>
	</xsl:otherwise>
    </xsl:choose>
  </xsl:element>
</xsl:template>

<xsl:template name="FeatureClassExtended">
    <xsl:element name="xs:complexContent">
      <xsl:element name="xs:extension">
        <xsl:attribute name="base" >
          <xsl:choose>
            <xsl:when test="@baseClass" >
              <!-- Has explicit base class. Extend from it -->
              <xsl:call-template name="base_class_to_type"/>
            </xsl:when>
            <xsl:otherwise>
              <!-- No explicit base, extend from gml base type -->
              <xsl:value-of select="'gml:AbstractFeatureType'" />
            </xsl:otherwise>
          </xsl:choose>
        </xsl:attribute>
        <!-- Write the properties as sub-elements -->
        <xsl:call-template name="class_properties" />
      </xsl:element>
    </xsl:element>
</xsl:template>

<xsl:template name="FeatureClassNotExtended">
   <!-- Write the properties as sub-elements -->
   <xsl:call-template name="class_properties" />
</xsl:template>

<!-- Converts a non-feature class to GML -->
<xsl:template match="Class" >
  <!-- Create a global element if class has identity properties
	 this makes it a stand-alone class
	-->
  <xsl:if test="IdentityProperties or UniqueConstraints" >
    <xsl:call-template name="global_element" />
  </xsl:if>

  <xsl:element name="xs:complexType">
    <xsl:call-template name="class_attributes" />
    <xsl:call-template name="element_subelements" />
    <xsl:choose>
      <xsl:when test="@baseClass or IdentityProperties" >
        <xsl:element name="xs:complexContent">
          <xsl:element name="xs:extension">
            <xsl:attribute name="base" >
              <xsl:choose>
                <xsl:when test="@baseClass" >
                  <!-- Has explicit base class. Extend from it -->
                  <xsl:call-template name="base_class_to_type"/>
                </xsl:when>
                <xsl:otherwise>
                  <!-- No explicit base, extend from fdo:ClassType to mark it as a non-feature class -->
                  <xsl:value-of select="'fdo:ClassType'" />
                </xsl:otherwise>
              </xsl:choose>
            </xsl:attribute>
            <!-- Write properties as sub-elements -->
            <xsl:call-template name="class_properties" />
          </xsl:element>
        </xsl:element>
      </xsl:when>
      <xsl:otherwise>
        <!-- No need to extend when no  id props and no base class -->
        <!-- Write properties as sub-elements -->
        <xsl:call-template name="class_properties" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:element>
</xsl:template>

<!-- Converts a network class to GML -->
<xsl:template match="NetworkClass" >
  <xsl:variable name="restrictedType">
    <xsl:value-of select="concat(@name,'_Restriction')" />
  </xsl:variable>
  <!-- Global element always required (even if no identity properties)-->
  <xsl:call-template name="global_element" />
  <!-- Write global element for layer class association -->
  <xsl:if test="@class">
    <xsl:call-template name="network_layer_member"/>
  </xsl:if>
  <!-- Write member property element for network primitive classes -->
  <xsl:call-template name="network_primitive_member" />
  
  <!-- Write restricted type, for restricting the classes that can be primitives of 
	     networks of this class 
	-->
  <xsl:element name="xs:complexType">
    <xsl:attribute name="name" >
      <xsl:value-of select="$restrictedType" />
    </xsl:attribute>
    <xsl:element name="xs:complexContent">
      <xsl:element name="xs:restriction">
        <xsl:choose>
          <!-- restriction is based on base class -->
          <xsl:when test="@baseClass" >
            <xsl:attribute name="base" >
              <xsl:call-template name="base_class_to_type"/>
            </xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="base">fdo:TopoComplexType</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:element name="xs:sequence">
          <!-- when restricting, must include all base elements,
			             start with common ones.
			        -->
          <xsl:call-template name="network_restrict"/>
          <!-- The rest are the gml:topoComplex elements -->
          <xsl:element name="xs:element">
            <xsl:attribute name="ref">gml:maximalComplex</xsl:attribute>
          </xsl:element>
          <xsl:element name="xs:element">
            <xsl:attribute name="ref">gml:superComplex</xsl:attribute>
            <xsl:attribute name="minOccurs">0</xsl:attribute>
            <xsl:attribute name="maxOccurs">0</xsl:attribute>
          </xsl:element>
          <!-- Subcomplexes allowed only if network class has a layer class -->
          <xsl:element name="xs:element">
            <xsl:choose>
              <xsl:when test="@class" >
                <!-- referencing LayerMember element restricts subcomplexes to the layer class -->
                <xsl:attribute name="ref">
                  <xsl:value-of select="concat(ancestor::Schema/@name,':',@name,'_LayerMember')" />
                </xsl:attribute>
                <xsl:attribute name="minOccurs">0</xsl:attribute>
                <xsl:attribute name="maxOccurs">unbounded</xsl:attribute>
              </xsl:when>
              <xsl:otherwise>
                <!-- No layer class, so no subcomplexes -->
                <xsl:attribute name="ref">gml:subComplex</xsl:attribute>
                <xsl:attribute name="minOccurs">0</xsl:attribute>
                <xsl:attribute name="maxOccurs">0</xsl:attribute>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:element>
          <!-- The following element restricts the network primitives to classes that 
					     are in this network class's primitive member substitution group.
					-->
          <xsl:element name="xs:element">
            <xsl:attribute name="ref">
              <xsl:value-of select="concat(ancestor::Schema/@name,':',@name,'_PrimitiveMember')"/>
            </xsl:attribute>
            <xsl:attribute name="minOccurs">0</xsl:attribute>
            <xsl:attribute name="maxOccurs">unbounded</xsl:attribute>
          </xsl:element>
          <xsl:element name="xs:element">
            <xsl:attribute name="ref">gml:topoPrimitiveMembers</xsl:attribute>
            <xsl:attribute name="minOccurs">0</xsl:attribute>
            <xsl:attribute name="maxOccurs">0</xsl:attribute>
          </xsl:element>
          <!-- Write the base properties since they must be included in a restriction -->
          <xsl:for-each select="BaseProperties">
            <xsl:apply-templates select="*" />
          </xsl:for-each>
        </xsl:element>
      </xsl:element>
    </xsl:element>
  </xsl:element>
  <!-- generate a complex type based on the restriction. Add the network class's properties here -->
  <xsl:element name="xs:complexType">
    <xsl:call-template name="class_attributes" />
    <xsl:call-template name="element_subelements" />
    <xsl:element name="xs:complexContent">
      <xsl:element name="xs:extension">
        <xsl:attribute name="base">
          <xsl:value-of select="concat(ancestor::Schema/@name,':',$restrictedType)" />
        </xsl:attribute>
        <!-- Write properties as sub-elements -->
        <xsl:call-template name="class_properties" />
      </xsl:element>
    </xsl:element>
  </xsl:element>
</xsl:template>

<!-- Converts a network layer class to GML -->
<xsl:template match="NetworkLayerClass" >
  <xsl:variable name="restrictedType">
    <xsl:value-of select="concat(@name,'_Restriction')" />
  </xsl:variable>
  <!-- Global element always required (even if no identity properties)-->
  <xsl:call-template name="global_element" />
  <!-- Write member property element for layer primitive classes -->
  <xsl:call-template name="network_primitive_member" />
  
  <!-- Write restricted type, for restricting the classes that can be primitives of 
         layers of this class 
    -->
  <xsl:element name="xs:complexType">
    <xsl:attribute name="name" >
      <xsl:value-of select="$restrictedType" />
    </xsl:attribute>
    <xsl:element name="xs:complexContent">
      <xsl:element name="xs:restriction">
        <xsl:choose>
          <xsl:when test="@baseClass" >
            <xsl:attribute name="base" >
              <xsl:call-template name="base_class_to_type"/>
            </xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="base">fdo:NetworkLayerType</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:element name="xs:sequence">
          <!-- when restricting, must include all base elements,
			             start with common ones.
			        -->
          <xsl:call-template name="network_restrict"/>
          <xsl:element name="xs:element">
            <xsl:attribute name="ref">gml:maximalComplex</xsl:attribute>
          </xsl:element>
          <!-- The rest are the gml:topoComplex elements -->
          <xsl:element name="xs:element">
            <xsl:attribute name="ref">gml:superComplex</xsl:attribute>
            <xsl:attribute name="minOccurs">0</xsl:attribute>
            <xsl:attribute name="maxOccurs">unbounded</xsl:attribute>
          </xsl:element>
          <xsl:element name="xs:element">
            <xsl:attribute name="ref">gml:subComplex</xsl:attribute>
            <xsl:attribute name="minOccurs">0</xsl:attribute>
            <xsl:attribute name="maxOccurs">0</xsl:attribute>
          </xsl:element>
          <!-- The following element restricts the layer primitives to classes that 
					     are in this layer class's primitive member substitution group.
					-->
          <xsl:element name="xs:element">
            <xsl:attribute name="ref">
              <xsl:value-of select="concat(ancestor::Schema/@name,':',@name,'_PrimitiveMember')"/>
            </xsl:attribute>
            <xsl:attribute name="minOccurs">0</xsl:attribute>
            <xsl:attribute name="maxOccurs">unbounded</xsl:attribute>
          </xsl:element>
          <xsl:element name="xs:element">
            <xsl:attribute name="ref">gml:topoPrimitiveMembers</xsl:attribute>
            <xsl:attribute name="minOccurs">0</xsl:attribute>
            <xsl:attribute name="maxOccurs">0</xsl:attribute>
          </xsl:element>
          <!-- Write the base properties since they must be included in a restriction -->
          <xsl:for-each select="BaseProperties">
            <xsl:apply-templates select="*" />
          </xsl:for-each>
        </xsl:element>
      </xsl:element>
    </xsl:element>
  </xsl:element>
  <!-- generate a complex type based on the restriction. Add the layer class's properties here -->
  <xsl:element name="xs:complexType">
    <xsl:call-template name="class_attributes" />
    <xsl:call-template name="element_subelements" />
    <xsl:element name="xs:complexContent">
      <xsl:element name="xs:extension">
        <xsl:attribute name="base">
          <xsl:value-of select="concat(ancestor::Schema/@name,':',$restrictedType)" />
        </xsl:attribute>
        <!-- Write properties as sub-elements -->
        <xsl:call-template name="class_properties" />
      </xsl:element>
    </xsl:element>
  </xsl:element>
</xsl:template>

<!-- Converts a network link feature class to GML -->
<xsl:template match="NetworkLinkFeatureClass" >
  <xsl:variable name="restrictedType">
    <xsl:value-of select="concat(@name,'_Restriction')" />
  </xsl:variable>
  <!-- Find out what kind of associations have been defined -->
  <xsl:variable name="refFeatProp">
    <xsl:for-each select="ReferenceFeatureProperty/AssociationProperty">
      <xsl:value-of select="@name" />
    </xsl:for-each>
  </xsl:variable>
  <xsl:variable name="parentPmtvProp">
    <xsl:for-each select="ParentNetworkProperty/AssociationProperty">
      <xsl:value-of select="@name" />
    </xsl:for-each>
  </xsl:variable>
  <xsl:variable name="startNodeProp">
    <xsl:for-each select="StartNode/AssociationProperty">
      <xsl:value-of select="@name" />
    </xsl:for-each>
  </xsl:variable>
  <xsl:variable name="endNodeProp">
    <xsl:for-each select="EndNode/AssociationProperty">
      <xsl:value-of select="@name" />
    </xsl:for-each>
  </xsl:variable>
  <!-- GML does not directly support the concept of start and end nodes, just related
         nodes. The following checks if the start and end node classes are different.
         We need to do an extra substitution group trick when they are different.
    -->
  <xsl:variable name="directedNodeProp">
    <xsl:choose>
      <xsl:when test="not(string-length($startNodeProp) = 0) and ($startNodeProp = $endNodeProp)">
        <!-- start and end node classes are the same -->
        <xsl:value-of select="$startNodeProp"/>
      </xsl:when>
      <!-- special value indicating that they are different -->
      <xsl:otherwise>DirectedNodeMember</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="needRestriction">
    <xsl:if test="$refFeatProp or $parentNetProp or $startNodeProp or $endNodeProp or not(@baseClass)">yes</xsl:if>
  </xsl:variable>
  <!-- Global element always required (even if no identity properties)-->
  <xsl:call-template name="global_element" />
  <!-- Write restricted type, for restricting the classes that can be associated to 
         links of this class. The restricted type is not needed if this class has no 
         feature, parent element, or node associations.
    -->
  <xsl:if test="$needRestriction">
    <xsl:call-template name="network_assocs_members">
      <xsl:with-param name="refFeatProp" select="$refFeatProp"/>
      <xsl:with-param name="parentPmtvProp" select="$parentPmtvProp"/>
      <xsl:with-param name="startNodeProp" select="$startNodeProp"/>
      <xsl:with-param name="endNodeProp" select="$endNodeProp"/>
      <xsl:with-param name="directedNodeProp" select="$directedNodeProp"/>
    </xsl:call-template>
    <xsl:element name="xs:complexType">
      <xsl:attribute name="name" >
        <xsl:value-of select="$restrictedType" />
      </xsl:attribute>
      <xsl:element name="xs:complexContent">
        <xsl:element name="xs:restriction">
          <xsl:choose>
            <xsl:when test="@baseClass" >
              <xsl:attribute name="base" >
                <xsl:call-template name="base_class_to_type"/>
              </xsl:attribute>
            </xsl:when>
            <xsl:otherwise>
              <!-- ancestor class for all links, based on gml:EdgeType -->
              <xsl:attribute name="base">fdo:EdgeType</xsl:attribute>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:element name="xs:sequence">
            <!-- restriction must contain all base elements. Start with 
				             ones common to network primitives
				        -->
            <xsl:call-template name="network_primitive_restrict" />
            <!-- Elements common to all gml:EdgeType classes -->
            <!-- Add element that restricts the associated node classes -->
            <xsl:element name="xs:element">
              <xsl:attribute name="ref">
                <xsl:choose>
                  <xsl:when test="(string-length($startNodeProp) = 0) and (string-length($endNodeProp) = 0)" >
                    <xsl:value-of select="'gml:directedNode'" />
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:value-of select="concat(ancestor::Schema/@name,':DirectedNode_',@name)" />
                  </xsl:otherwise>
                </xsl:choose>
              </xsl:attribute>
              <xsl:choose>
                <xsl:when test="not(string-length($startNodeProp) = 0) and not(string-length($endNodeProp) = 0)" >
                  <!-- link class has both start and end node -->
                  <xsl:attribute name="minOccurs">2</xsl:attribute>
                  <xsl:attribute name="maxOccurs">2</xsl:attribute>
                </xsl:when>
                <xsl:when test="(string-length($startNodeProp) = 0) and (string-length($endNodeProp) = 0)" >
                  <!-- link class has no associated nodes -->
                  <xsl:attribute name="minOccurs">0</xsl:attribute>
                  <xsl:attribute name="maxOccurs">0</xsl:attribute>
                </xsl:when>
                <xsl:otherwise >
                  <!-- has start or end node but not both -->
                  <xsl:attribute name="minOccurs">1</xsl:attribute>
                  <xsl:attribute name="maxOccurs">1</xsl:attribute>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:element>
            <xsl:element name="xs:element">
              <xsl:attribute name="ref">gml:directedFace</xsl:attribute>
              <xsl:attribute name="minOccurs">0</xsl:attribute>
              <xsl:attribute name="maxOccurs">0</xsl:attribute>
            </xsl:element>
            <xsl:element name="xs:element">
              <xsl:attribute name="ref">gml:curveProperty</xsl:attribute>
              <xsl:attribute name="minOccurs">0</xsl:attribute>
              <xsl:attribute name="maxOccurs">0</xsl:attribute>
            </xsl:element>
            <!-- elements common to all fdo:EdgeType classes -->
            <!-- the following element restricts the classes for associated features -->
            <xsl:element name="xs:element">
              <xsl:attribute name="ref">
                <xsl:choose>
                  <xsl:when test="not(string-length($refFeatProp) = 0)" >
                    <xsl:value-of select="concat(ancestor::Schema/@name,':ReferenceFeature_',@name)" />
                  </xsl:when>
                  <xsl:otherwise>gml:featureProperty</xsl:otherwise>
                </xsl:choose>
              </xsl:attribute>
              <xsl:choose>
                <xsl:when test="string-length($refFeatProp) = 0">
                  <xsl:attribute name="minOccurs">0</xsl:attribute>
                  <xsl:attribute name="maxOccurs">0</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:attribute name="minOccurs">0</xsl:attribute>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:element>
            <!-- the following element restricts the classes for associated parent link -->
            <xsl:element name="xs:element">
              <xsl:attribute name="ref">
                <xsl:choose>
                  <xsl:when test="not(string-length($parentPmtvProp)= 0)" >
                    <xsl:value-of select="concat(ancestor::Schema/@name,':ParentPrimitive_',@name)" />
                  </xsl:when>
                  <xsl:otherwise>gml:topoPrimitiveMember</xsl:otherwise>
                </xsl:choose>
              </xsl:attribute>
              <xsl:choose>
                <xsl:when test="string-length($parentPmtvProp) = 0">
                  <xsl:attribute name="minOccurs">0</xsl:attribute>
                  <xsl:attribute name="maxOccurs">0</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:attribute name="minOccurs">0</xsl:attribute>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:element>
            <!-- base class properties must also be present -->
            <xsl:for-each select="BaseProperties">
              <xsl:apply-templates select="*" />
            </xsl:for-each>
          </xsl:element>
        </xsl:element>
      </xsl:element>
    </xsl:element>
  </xsl:if>
  
  <!-- generate a complex type based on the restriction. Add the link class's properties here -->
  <xsl:element name="xs:complexType">
    <xsl:call-template name="network_pmtv_attributes" />
    <xsl:call-template name="element_subelements" />
    <xsl:element name="xs:complexContent">
      <xsl:element name="xs:extension">
        <xsl:attribute name="base" >
          <xsl:choose>
            <xsl:when test="$needRestriction" >
              <!-- extend from restriction class -->
              <xsl:value-of select="concat(ancestor::Schema/@name,':',$restrictedType)" />
            </xsl:when>
            <xsl:otherwise>
              <!-- No restriction, extend from base class. -->
              <xsl:call-template name="base_class_to_type"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:attribute>
        <!-- Write properties as sub-elements -->
        <xsl:call-template name="class_properties" />
      </xsl:element>
    </xsl:element>
  </xsl:element>
</xsl:template>

<!-- Converts a network node feature class to GML -->
<xsl:template match="NetworkNodeFeatureClass" >
  <xsl:variable name="restrictedType">
    <xsl:value-of select="concat(@name,'_Restriction')" />
  </xsl:variable>
  <!-- Find out what kind of associations have been defined -->
  <xsl:variable name="refFeatProp">
    <xsl:for-each select="ReferenceFeatureProperty/AssociationProperty">
      <xsl:value-of select="@name" />
    </xsl:for-each>
  </xsl:variable>
  <xsl:variable name="parentPmtvProp">
    <xsl:for-each select="ParentNetworkProperty/AssociationProperty">
      <xsl:value-of select="@name" />
    </xsl:for-each>
  </xsl:variable>
  <xsl:variable name="needRestriction">
    <xsl:if test="$refFeatProp or $parentNetProp or not(@baseClass)">yes</xsl:if>
  </xsl:variable>
  <!-- Global element always required (even if no identity properties)-->
  <xsl:call-template name="global_element" />
  <!-- Write restricted type, for restricting the classes that can be associated to 
         nodes of this class. The restricted type is not needed if this class has no 
         feature or parent element.
    -->
  <xsl:if test="$needRestriction">
    <xsl:call-template name="network_assocs_members">
      <xsl:with-param name="refFeatProp">
        <xsl:value-of select="$refFeatProp"/>
      </xsl:with-param>
      <xsl:with-param name="parentPmtvProp">
        <xsl:value-of select="$parentPmtvProp"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:element name="xs:complexType">
      <xsl:attribute name="name" >
        <xsl:value-of select="$restrictedType" />
      </xsl:attribute>
      <xsl:element name="xs:complexContent">
        <xsl:element name="xs:restriction">
          <xsl:choose>
            <xsl:when test="@baseClass" >
              <xsl:attribute name="base" >
                <xsl:call-template name="base_class_to_type"/>
              </xsl:attribute>
            </xsl:when>
            <xsl:otherwise>
              <xsl:attribute name="base">fdo:NodeType</xsl:attribute>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:element name="xs:sequence">
            <!-- restriction must contain all base elements. Start with 
				             ones common to network primitives
				        -->
            <xsl:call-template name="network_primitive_restrict" />
            <xsl:element name="xs:element">
              <xsl:attribute name="ref">gml:directedEdge</xsl:attribute>
              <xsl:attribute name="minOccurs">0</xsl:attribute>
              <xsl:attribute name="maxOccurs">0</xsl:attribute>
            </xsl:element>
            <xsl:element name="xs:element">
              <xsl:attribute name="ref">gml:pointProperty</xsl:attribute>
              <xsl:attribute name="minOccurs">0</xsl:attribute>
              <xsl:attribute name="maxOccurs">0</xsl:attribute>
            </xsl:element>
            <!-- Add elements for fdo:NodeType -->
            <!-- Element to restrict the class for the related feature -->
            <xsl:element name="xs:element">
              <xsl:attribute name="ref">
                <xsl:choose>
                  <xsl:when test="not(string-length($refFeatProp) = 0)" >
                    <xsl:value-of select="concat(ancestor::Schema/@name,':ReferenceFeature_',@name)" />
                  </xsl:when>
                  <xsl:otherwise>gml:featureProperty</xsl:otherwise>
                </xsl:choose>
              </xsl:attribute>
              <xsl:choose>
                <xsl:when test="string-length($refFeatProp) = 0">
                  <xsl:attribute name="minOccurs">0</xsl:attribute>
                  <xsl:attribute name="maxOccurs">0</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:attribute name="minOccurs">0</xsl:attribute>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:element>
            <!-- Element to restrict the class for the related parent node -->
            <xsl:element name="xs:element">
              <xsl:attribute name="ref">
                <xsl:choose>
                  <xsl:when test="not(string-length($parentPmtvProp)= 0)" >
                    <xsl:value-of select="concat(ancestor::Schema/@name,':ParentPrimitive_',@name)" />
                  </xsl:when>
                  <xsl:otherwise>gml:topoPrimitiveMember</xsl:otherwise>
                </xsl:choose>
              </xsl:attribute>
              <xsl:choose>
                <xsl:when test="string-length($parentPmtvProp) = 0">
                  <xsl:attribute name="minOccurs">0</xsl:attribute>
                  <xsl:attribute name="maxOccurs">0</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:attribute name="minOccurs">0</xsl:attribute>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:element>
            <!-- base class properties must also be present -->
            <xsl:for-each select="BaseProperties">
              <xsl:apply-templates select="*" />
            </xsl:for-each>
          </xsl:element>
        </xsl:element>
      </xsl:element>
    </xsl:element>
  </xsl:if>
  
  <!-- generate a complex type based on the restriction. Add the network class's properties here -->
  <xsl:element name="xs:complexType">
    <xsl:call-template name="network_pmtv_attributes" />
    <xsl:call-template name="element_subelements" />
    <xsl:element name="xs:complexContent">
      <xsl:element name="xs:extension">
        <xsl:attribute name="base" >
          <xsl:choose>
            <xsl:when test="$needRestriction" >
              <!-- extend from restriction class -->
              <xsl:value-of select="concat(ancestor::Schema/@name,':',$restrictedType)" />
            </xsl:when>
            <xsl:otherwise>
              <!-- No restriction, extend from base class. -->
              <xsl:call-template name="base_class_to_type"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:attribute>
        <!-- Write properties as sub-elements -->
        <xsl:call-template name="class_properties" />
      </xsl:element>
    </xsl:element>
  </xsl:element>
</xsl:template>
  
<!-- Converts a data property to GML -->
<xsl:template match="DataProperty" >
	<xsl:element name="xs:element">
		<xsl:call-template name="element_attributes" />
		<xsl:if test="@readOnly" >
			<xsl:attribute name="fdo:readOnly" >
				<xsl:value-of select="@readOnly" />
			</xsl:attribute>
		</xsl:if>
		<xsl:if test="@nullable='true'" >
			<xsl:attribute name="minOccurs" >
				<xsl:value-of select="'0'" />
			</xsl:attribute>
		</xsl:if>
		<xsl:if test="not(string-length(@default) = 0)" >
			<xsl:attribute name="default" >
				<xsl:value-of select="@default" />
			</xsl:attribute>
		</xsl:if>
		<xsl:if test="@autogenerated" >
			<xsl:attribute name="fdo:autogenerated" >
				<xsl:value-of select="@autogenerated" />
			</xsl:attribute>
		</xsl:if>
		<xsl:choose>
			<xsl:when test="@dataType = 'string'" >
				<xsl:call-template name="dataproperty_string" />
			</xsl:when>
			<xsl:when test="@dataType = 'decimal'" >
				<xsl:call-template name="dataproperty_decimal" />
			</xsl:when>
			<xsl:when test="@dataType = 'boolean'" >
				<xsl:call-template name="dataproperty_other" >
					<xsl:with-param name="gmlType" >xs:boolean</xsl:with-param>
				</xsl:call-template>
			</xsl:when>
			<xsl:when test="@dataType = 'byte'" >
                <xsl:call-template name="dataproperty_other" >
                    <xsl:with-param name="gmlType" >xs:unsignedByte</xsl:with-param>
                </xsl:call-template>
			</xsl:when>
			<xsl:when test="@dataType = 'datetime'" >
				<xsl:call-template name="dataproperty_other" >
					<xsl:with-param name="gmlType" >xs:dateTime</xsl:with-param>
				</xsl:call-template>
			</xsl:when>
			<xsl:when test="@dataType = 'double'" >
				<xsl:call-template name="dataproperty_other" >
					<xsl:with-param name="gmlType" >xs:double</xsl:with-param>
				</xsl:call-template>
			</xsl:when>
			<xsl:when test="@dataType = 'int16'" >
				<xsl:call-template name="dataproperty_other" >
					<xsl:with-param name="gmlType" >xs:short</xsl:with-param>
				</xsl:call-template>
			</xsl:when>
			<xsl:when test="@dataType = 'int32'" >
				<xsl:call-template name="dataproperty_other" >
					<xsl:with-param name="gmlType" >xs:int</xsl:with-param>
				</xsl:call-template>
			</xsl:when>
			<xsl:when test="@dataType = 'int64'" >
				<xsl:call-template name="dataproperty_other" >
					<xsl:with-param name="gmlType" >xs:long</xsl:with-param>
				</xsl:call-template>
			</xsl:when>
			<xsl:when test="@dataType = 'single'" >
				<xsl:call-template name="dataproperty_other" >
					<xsl:with-param name="gmlType" >xs:float</xsl:with-param>
				</xsl:call-template>
			</xsl:when>
			<xsl:when test="@dataType = 'blob'" >
				<xsl:call-template name="dataproperty_other" >
					<xsl:with-param name="gmlType" >xs:hexBinary</xsl:with-param>
				</xsl:call-template>
			</xsl:when>
			<xsl:when test="@dataType = 'clob'" >
				<xsl:call-template name="dataproperty_other" >
					<xsl:with-param name="gmlType" >fdo:clob</xsl:with-param>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:call-template name="dataproperty_other" >
					<xsl:with-param name="gmlType" >xs:string</xsl:with-param>
				</xsl:call-template>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:element>
</xsl:template>

<!-- Converts a geometric property to GML -->
<xsl:template match="GeometricProperty" >
	<xsl:element name="xs:element">
		<xsl:call-template name="element_attributes" />
		<xsl:attribute name="type" >gml:AbstractGeometryType</xsl:attribute>
		<xsl:if test="@readOnly">
			<xsl:attribute name="fdo:geometryReadOnly" >
				<xsl:value-of select="@readOnly" />
			</xsl:attribute>
		</xsl:if>
		<xsl:call-template name="geometricproperty_attributes" />
		<xsl:call-template name="element_subelements" />
	</xsl:element>
</xsl:template>

<!-- Converts an object property to GML -->
<xsl:template match="ObjectProperty" >
	<xsl:element name="xs:element">
		<xsl:call-template name="element_attributes" />
		<xsl:attribute name="type" >
            <xsl:call-template name="class_to_type">
                <xsl:with-param name="schema">
                    <xsl:call-template name="choose_schemaPrefix">
                        <xsl:with-param name="schema" select="@classSchema" />
                        <xsl:with-param name="prefix" select="//Schema[@name=@classSchema]/@prefix" />
                    </xsl:call-template>
                </xsl:with-param>
                <xsl:with-param name="class" select="@class"/>
            </xsl:call-template>
		</xsl:attribute>
		<xsl:attribute name="minOccurs" >0</xsl:attribute>
		<xsl:choose>
			<xsl:when test="@objectType='value'" />
			<xsl:otherwise>
				<xsl:attribute name="maxOccurs" >unbounded</xsl:attribute>
				<xsl:if test="@identityProperty" >
					<xsl:attribute name="fdo:identityProperty" >
						<xsl:value-of select="@identityProperty" />
					</xsl:attribute>
				</xsl:if>
				<xsl:if test="@objectType = 'orderedcollection'" >
					<xsl:attribute name="fdo:order" >
						<xsl:value-of select="@orderType" />
					</xsl:attribute>
				</xsl:if>
			</xsl:otherwise>
		</xsl:choose>
		<xsl:call-template name="element_subelements" />
	</xsl:element>
</xsl:template>

<!-- Converts an association property to GML -->
<xsl:template match="AssociationProperty" >
	<xsl:element name="xs:element">
		<xsl:call-template name="element_attributes" />
		<xsl:attribute name="minOccurs" >0</xsl:attribute>
       	<xsl:attribute name="maxOccurs" >1</xsl:attribute>
		<xsl:call-template name="association_attributes"/>
		<!-- Write a special attribute to indicate when the identity properties
		 are explicit. On import, then are implicit by default
		-->
		<xsl:if test="not(IdentityProperties[@default='true'])" >
			<xsl:attribute name="fdo:defaultIdentity" >false</xsl:attribute>
		</xsl:if>
		<xsl:call-template name="element_subelements" />
        <xsl:choose>
			<xsl:when test="IdentityProperties[@default='true']">
				<!-- Association has default identity properties (correspond to associated class identity properties. 
					Write association in GML format.

					Association type is written inline. GML files commonly define a non-inline type but inline is 
					easier to handle. 
				-->
				<xsl:element name="xs:complexType">
					<xsl:element name="xs:complexContent">
						<!-- Association element type is restriction on the GML feature property type -->
						<xsl:element name="xs:restriction">
							<xsl:attribute name="base">
								<xsl:value-of select="$g_featurePropertyType"/>
							</xsl:attribute>
							<xsl:element name="xs:sequence">
								<!-- 1st element of feature property type defines the assocation class type. Therefore, redefine this element
										to have the same type as the associated class.
								-->
								<xsl:element name="xs:element">
									<xsl:attribute name="ref">
										<xsl:call-template name="assoc_property_ref"/>
									</xsl:attribute>
									<xsl:attribute name="minOccurs">0</xsl:attribute>
								</xsl:element>
							</xsl:element>
							<xsl:choose>
								<!-- Attributes depend on GML version -->
								<xsl:when test="$g_featurePropertyType = 'gml:FeaturePropertyType'">
									<!-- GML 3 attributes -->
									<xsl:element name="xs:attributeGroup">
										<xsl:attribute name="ref">gml:AssociationAttributeGroup</xsl:attribute>
									</xsl:element>
								</xsl:when>
								<xsl:otherwise>
									<!-- GML 2 attributes -->
									<xsl:element name="xs:attributeGroup">
										<xsl:attribute name="ref">xlink:simpleLink</xsl:attribute>
									</xsl:element>
									<xsl:element name="xs:attribute">
										<xsl:attribute name="ref">gml:remoteSchema</xsl:attribute>
										<xsl:attribute name="use">prohibited</xsl:attribute>
									</xsl:element>
								</xsl:otherwise>
							</xsl:choose>
						</xsl:element>
					</xsl:element>
				</xsl:element>
			</xsl:when>
			<xsl:otherwise>
				<!-- non-default identity properties cannot be handled by GML assocation so use
					XML schema keyref instead.
				-->
			
				<!-- The reverse identity properties become sub-properties of this element -->
				<xsl:element name="xs:complexType">
					<xsl:element name="xs:sequence">
						<xsl:apply-templates select="IdentityReverseProperties/node()" />
					</xsl:element>
				</xsl:element>
				<!-- The associated class and identity properties are referenced by 
				 a keyref. The keyref points to a xs:key or xs:unique key for a global
				 element whose type is the associated class. The identity properties are
				 represented as fields of the keyref.
				-->
				<xsl:element name="xs:keyref">
					<xsl:attribute name="name" >
					<xsl:value-of select="concat(ancestor::Schema/@name,'_',../../@name,'_',@name)" />
					</xsl:attribute>
					<!-- refer points to the referenced xs:key or xs:unique. assoc_property_key
					 makes sure the right one is referenced.
					-->
					<xsl:attribute name="refer" >
						<xsl:call-template name="assoc_property_key"/>
					</xsl:attribute>
					<xsl:element name="xs:selector">
						<!-- TODO: selector path currently not used so it doesn't matter what it is 
						 set to. However, need to check if it is set correctly at some point.
						-->
						<xsl:attribute name="xpath" >
							<xsl:value-of select="concat('.//', ../../@name)" />
						</xsl:attribute>
					</xsl:element>
					<xsl:for-each select="IdentityProperties/IdentityProperty">
						<xsl:element name="xs:field">
							<xsl:attribute name="xpath" >
								<xsl:value-of select="." />
							</xsl:attribute>
						</xsl:element>
					</xsl:for-each>
				</xsl:element>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:element>
</xsl:template>

<!-- Converts a raster property to GML -->
<xsl:template match="RasterProperty" >
	<xsl:element name="xs:element">
		<xsl:call-template name="element_attributes" />
       	<xsl:attribute name="type">fdo:RasterPropertyType</xsl:attribute>
       	<!-- Write out the top-level attributes. The default data model is handled
       	 inside element_subelements.
       	-->
    	<xsl:if test="@nullable = 'true'" >
           	<xsl:attribute name="minOccurs" >0</xsl:attribute>
        </xsl:if>
		<xsl:if test="@readOnly" >
			<xsl:attribute name="fdo:readOnly" >
				<xsl:value-of select="@readOnly" />
			</xsl:attribute>
		</xsl:if>
		<xsl:if test="@sizeX" >
			<xsl:attribute name="fdo:defaultImageXSize" >
				<xsl:value-of select="@sizeX" />
			</xsl:attribute>
		</xsl:if>
		<xsl:if test="@sizeY" >
			<xsl:attribute name="fdo:defaultImageYSize" >
				<xsl:value-of select="@sizeY" />
			</xsl:attribute>
		</xsl:if>
        <xsl:if test="@srsName">
            <xsl:attribute name="fdo:srsName" >
                <xsl:value-of select="@srsName" />
            </xsl:attribute>
        </xsl:if>
        <xsl:call-template name="element_subelements" />
	</xsl:element>
</xsl:template>

<!-- Description and SAD handled under element_subelements -->
<xsl:template match="Description|SAD" >
</xsl:template>

<!-- Writes a namespace declaration from the given schema name -->
<xsl:template name="declare_namespace">
    <xsl:param name="schema"/>
    
	<xsl:variable name="prefix" >
	    <xsl:call-template name="schema_to_prefix">
            <xsl:with-param name="schema">
                <xsl:call-template name="choose_schemaPrefix">
                    <xsl:with-param name="schema" select="$schema" />
                    <xsl:with-param name="prefix" select="//Schema[@name=$schema]/@prefix" />
                </xsl:call-template>
            </xsl:with-param>
	    </xsl:call-template>
	</xsl:variable>

    <xsl:variable name="uri" >
        <xsl:call-template name="choose_targetNamespace">
            <xsl:with-param name="schema" select="$schema"/>
            <xsl:with-param name="custom_targetNamespace" select="//Schema[@name=$schema]/@targetNamespace"/>
        </xsl:call-template>
	</xsl:variable>

    <xsl:attribute name="{concat('xmlns:',$prefix)}" >
		<xsl:value-of select="$uri" />
	</xsl:attribute>
</xsl:template>
		
<!-- Writes a global element for a class. The global element indicates the 
 identity properties. It also provides the link between each association property
 and its associated class.
-->
<xsl:template name="global_element">
	<xsl:element name="xs:element">
    <xsl:variable name="className" select="@name" />
    <xsl:attribute name="name" >
			<xsl:value-of select="@name" />
		</xsl:attribute>
		<!-- TODO: support type with implicit schema -->
		<xsl:attribute name="type" >
		    <xsl:call-template name="class_to_type">
                <xsl:with-param name="schema">
                    <xsl:call-template name="choose_schemaPrefix">
                        <xsl:with-param name="schema" select="ancestor::Schema/@name" />
                        <xsl:with-param name="prefix" select="ancestor::Schema/@prefix" />
                    </xsl:call-template>
                </xsl:with-param>
		        <xsl:with-param name="class" select="@name"/>
		    </xsl:call-template>
		</xsl:attribute>
		<xsl:attribute name="abstract" >
			<xsl:value-of select="@abstract" />
		</xsl:attribute>
		<xsl:attribute name="substitutionGroup" >
		    <xsl:choose>
		        <xsl:when test="local-name() = 'NetworkClass'">
		            <!-- Networks map onto gml TopoComplexes -->
        			<xsl:value-of select="'gml:TopoComplex'" />
		        </xsl:when>
		        <xsl:when test="local-name() = 'NetworkLayerClass'">
		            <!-- so do layers but need a special element to distinguish from layers -->
        			<xsl:value-of select="'fdo:NetworkLayer'" />
		        </xsl:when>
                <!-- For network links and nodes, the substitution group is set to the 
                     global element for the associated network's primitives. This restricts the 
                     networks that these primitives can belong to -->
		        <xsl:when test="local-name() = 'NetworkLinkFeatureClass' or local-name() = 'NetworkNodeFeatureClass'">
                    <xsl:choose>
                        <xsl:when test="@network">
                            <xsl:variable name="network" select="@network" />
                            <xsl:for-each select="NetworkProperty/AssociationProperty[@name=$network]">
                                <xsl:value-of select="concat(@associatedClassSchema,':',@associatedClass,'_Primitive')" />
                            </xsl:for-each>
                        </xsl:when>
                        <!-- When no associated network, just default to network primitive substitution group -->
                        <xsl:otherwise>fdo:NetworkPrimitiveMember</xsl:otherwise>    
                    </xsl:choose>
		        </xsl:when>
		        <xsl:otherwise>
        			<xsl:value-of select="'gml:_Feature'" />
		        </xsl:otherwise>
		    </xsl:choose>			
		</xsl:attribute>
		<!-- For nodes and links, we need to store the association property defining the 
		     associated network. This is done by special attributes on the global element.
		     These attributes are optional and defaults are generated on deserialization.
		-->
        <xsl:if test="local-name() = 'NetworkLinkFeatureClass' or local-name() = 'NetworkNodeFeatureClass'">
            <xsl:if test="@network">
                <xsl:variable name="network" select="@network" />
                <xsl:for-each select="NetworkProperty/AssociationProperty[@name=$network]">
                    <xsl:attribute name="fdo:assocPropName">
                        <xsl:value-of select="@name" />
                    </xsl:attribute>
            		<xsl:call-template name="association_attributes"/>
                </xsl:for-each>
            </xsl:if>
        </xsl:if>
		<xsl:for-each select="IdentityProperties" >
			<!-- Identity properties kept in a key -->
			<xsl:element name="xs:key" >
				<xsl:attribute name="name" >
					<xsl:value-of select="concat(../@name,'Key')" />
				</xsl:attribute>
				<xsl:element name="xs:selector" >
					<xsl:attribute name="xpath" >
						<xsl:value-of select="concat('.//',../@name)" />
					</xsl:attribute>
				</xsl:element>
				<xsl:for-each select="IdentityProperty" >
					<xsl:element name="xs:field" >
						<xsl:attribute name="xpath" >
							<xsl:value-of select="." />
						</xsl:attribute>
					</xsl:element>
				</xsl:for-each>
			</xsl:element>
		</xsl:for-each>
		<xsl:variable name="elemSelector" select="concat('.//',../@name)" />
		<xsl:variable name="elemSchema" select="../@name" />
  		<!-- Create an xs:unique for each association property whose associate
  		 class is the type of this element and whose identity properties are 
  		 explicit (not the associated class identity properties). When identity
  		 properties are explicit, the already generated xs:key links the association
  		 property to its associated class.
  		-->
  		<xsl:if test="not (count($g_associationProperties) = 0)">
  		   <xsl:for-each select="key('assocKey',concat($elemSchema,':',@name))" >
  		      <!-- Skip if schema for this element and the association property are 
  		       not the same. When they are different, a separate external element is generated.
  		      -->
                  <xsl:if test="IdentityProperties and ($elemSchema = ancestor::Schema/@name)">
                    <!-- Skip association properties with implicit identity properties, these
                     are already linked through this element's xs:key
                    -->
                    <xsl:if test="not(IdentityProperties/@default)">
                      <!-- Generate the xs:unique that will link the association property to 
                       its associated class. This xs:unique is referenced by the association 
                       property's xs:keyref.
                      -->
			        <xsl:element name="xs:key" >
  				        <xsl:attribute name="name" >
  			                <xsl:call-template name="assoc_property_key">
  			                    <xsl:with-param name="includeSchema">false</xsl:with-param>
  			                </xsl:call-template>
  				        </xsl:attribute>
  				        <xsl:element name="xs:selector" >
					        <xsl:attribute name="xpath" >
						        <xsl:value-of select="$elemSelector" />
					        </xsl:attribute>
				        </xsl:element>
				        <xsl:for-each select="IdentityProperties/IdentityProperty" >
					        <xsl:element name="xs:field" >
						        <xsl:attribute name="xpath" >
							        <xsl:value-of select="." />
						        </xsl:attribute>
					        </xsl:element>
				        </xsl:for-each>
			        </xsl:element>
                </xsl:if>
            </xsl:if>
         </xsl:for-each>
	</xsl:if>

    <!-- Write the unique Constraints -->
    <xsl:for-each select="./UniqueConstraints/UniqueConstraint">
      <xsl:element name="xs:unique">
        <xsl:attribute name="name">
          <xsl:value-of select="@name" />
        </xsl:attribute>
        <!--<xsl:attribute name="type">Constraint</xsl:attribute>-->
        <xsl:element name="xs:selector">
          <xsl:attribute name="xpath">
            <xsl:value-of select="concat('.//', $className)" />
          </xsl:attribute>
        </xsl:element>
        <xsl:for-each select="./ConstraintProperty">
          <xsl:element name="xs:field">
            <xsl:attribute name="xpath">
              <xsl:value-of select="@name" />
            </xsl:attribute>
          </xsl:element>
        </xsl:for-each>
      </xsl:element>
    </xsl:for-each>
    
	</xsl:element>
	<!-- For node classes with an associated layer class, we need an extra global element
	     to define this association 
	-->
	<xsl:if test="local-name() = 'NetworkNodeFeatureClass' and (@LayerProperty)">
        <xsl:variable name="LayerProperty" select="@LayerProperty" />
	    <xsl:element name="xs:element">
		    <xsl:attribute name="name" >
			    <xsl:value-of select="concat('NodeLayer_',@name)" />
		    </xsl:attribute>
		    <!-- TODO: support type with implicit schema -->
		    <xsl:attribute name="type" >
                <xsl:call-template name="class_to_type">
                    <xsl:with-param name="schema">
                        <xsl:call-template name="choose_schemaPrefix">
                            <xsl:with-param name="schema" select="ancestor::Schema/@name" />
                            <xsl:with-param name="prefix" select="ancestor::Schema/@prefix" />
                        </xsl:call-template>
                    </xsl:with-param>
                    <xsl:with-param name="class" select="@name"/>
                </xsl:call-template>
		    </xsl:attribute>
		    <xsl:attribute name="abstract" >
			    <xsl:value-of select="@abstract" />
		    </xsl:attribute>
		    <!-- Set substitution group to the global element for the layer class's primitives -->
		    <xsl:attribute name="substitutionGroup" >
                <xsl:choose>
                    <xsl:when test="@LayerProperty">
                        <xsl:for-each select="AssociationProperty[@name=$LayerProperty]">
                            <xsl:value-of select="concat(@associatedClassSchema,':',@associatedClass,'_Primitive')" />
                        </xsl:for-each>
                    </xsl:when>
                    <xsl:otherwise>fdo:LayerPrimitiveMember</xsl:otherwise>    
                </xsl:choose>
    	    </xsl:attribute>
    	    <!-- store the layer association property as optional attributes on the global element -->
            <xsl:for-each select="AssociationProperty[@name=$LayerProperty]">
                <xsl:attribute name="fdo:assocPropName">
                    <xsl:value-of select="@name" />
                </xsl:attribute>
         		<xsl:call-template name="association_attributes"/>
            </xsl:for-each>
    	</xsl:element>
	</xsl:if>
</xsl:template>

<!-- Writes a global member for a class. The global member allows the class to be
     embedded or referenced by a property.
     
     This template is currently not called but is kept in case it is in the future.
-->
<xsl:template name="global_member">
    <!-- Create global element for the member property definition -->
	<xsl:element name="xs:element">
    <xsl:variable name="className" select="@name" />
    <xsl:attribute name="name" >
			<xsl:value-of select="concat(@name,'_Member')" />
		</xsl:attribute>
		<!-- TODO: support type with implicit schema -->
		<xsl:attribute name="type" >
			<xsl:value-of select="concat(ancestor::Schema/@name,':',@name,'_MemberType')" />
		</xsl:attribute>
		<xsl:attribute name="abstract" >false</xsl:attribute>
		<xsl:attribute name="substitutionGroup" >
		    <xsl:choose>
		        <xsl:when test="local-name() = 'NetworkClass'">
        			<xsl:value-of select="'gml:TopoComplexProperty'" />
		        </xsl:when>
		        <xsl:when test="local-name() = 'NetworkLayerClass'">
        			<xsl:value-of select="'fdo:NetworkLayerMember'" />
		        </xsl:when>
		        <xsl:when test="local-name() = 'NetworkLinkFeatureClass' or local-name() = 'NetworkNodeFeatureClass'">
        			<xsl:value-of select="'gml:TopoPrimitiveProperty'" />
		        </xsl:when>
		        <xsl:otherwise>
        			<xsl:value-of select="'gml:featureProperty'" />
		        </xsl:otherwise>
		    </xsl:choose>			
		</xsl:attribute>
	</xsl:element>
	<!-- Create the type for the global member property -->
    <xsl:element name="xs:complexType">
		<xsl:attribute name="name" >
			<xsl:value-of select="concat(@name,'_MemberType')" />
		</xsl:attribute>
		<xsl:attribute name="abstract">false</xsl:attribute>
		<xsl:element name="xs:complexContent">
		    <xsl:element name="xs:restriction">
		        <!-- base it on the right GML property member type for the FDO class -->
		        <xsl:attribute name="base">
		            <xsl:choose>
		                <xsl:when test="local-name() = 'NetworkClass' or local-name() = 'NetworkLayerClass'">
        			        <xsl:value-of select="'gml:TopoComplexMemberType'" />
		                </xsl:when>
		                <xsl:when test="local-name() = 'NetworkLinkFeatureClass' or local-name() = 'NetworkNodeFeatureClass'">
        			        <xsl:value-of select="'gml:topoPrimitiveMemberType'"/>
		                </xsl:when>
		                <xsl:otherwise>
        			        <xsl:value-of select="'gml:FeaturePropertyType'" />
		                </xsl:otherwise>
		            </xsl:choose>			
		        </xsl:attribute>
		        <xsl:element name="xs:sequence">
		            <!-- Set up the reference between the member property and the current FDO class -->
		            <xsl:element name="xs:element">
		                <xsl:attribute name="ref">
		                    <xsl:value-of select="concat(ancestor::Schema/@name,':',@name)" />
		                </xsl:attribute>
		            </xsl:element>
		        </xsl:element>
                <xsl:element name="xs:attributeGroup">
                    <xsl:attribute name="ref">gml:AssociationAttributeGroup</xsl:attribute>
                </xsl:element>
		    </xsl:element>
		</xsl:element>
    </xsl:element>
</xsl:template>

<!-- Writes an assocation between a network class and a network layer class. -->
<xsl:template name="network_layer_member">
    <!-- Write a global element for the assocation type -->
	<xsl:element name="xs:element">
		<xsl:attribute name="name" >
			<xsl:value-of select="concat(@name,'_LayerMember')" />
		</xsl:attribute>
		<!-- TODO: support type with implicit schema -->
		<xsl:attribute name="type" >
		    <xsl:value-of select="concat(ancestor::Schema/@name,':',@name,'_LayerMemberType')"/>
		</xsl:attribute>
		<xsl:attribute name="abstract" >false</xsl:attribute>
	</xsl:element>
    <!-- Write the assocation as a GML property member type -->
    <xsl:element name="xs:complexType">
		<xsl:attribute name="name" >
			<xsl:value-of select="concat(@name,'_LayerMemberType')" />
		</xsl:attribute>
		<xsl:attribute name="abstract">false</xsl:attribute>
		<xsl:element name="xs:complexContent">
		    <xsl:element name="xs:restriction">
		        <xsl:attribute name="base">gml:TopoComplexMemberType</xsl:attribute>
		        <xsl:element name="xs:sequence">
		            <xsl:element name="xs:element">
		                <!-- set the property member to reference the layer class -->
		                <xsl:attribute name="ref">
		                    <xsl:value-of select="concat(@classSchema,':',@class)" />
		                </xsl:attribute>
		            </xsl:element>
		        </xsl:element>
                <xsl:element name="xs:attributeGroup">
                    <xsl:attribute name="ref">gml:AssociationAttributeGroup</xsl:attribute>
                </xsl:element>
		    </xsl:element>
		</xsl:element>
    </xsl:element>
</xsl:template>

<!-- Generates a GML member property representing the valid primitives for a network or network 
     layer class.
-->
<xsl:template name="network_primitive_member">
    <!-- Write the network primitive global element. The valid network primitive classes are all classes whose 
         global element has the following element as its substitution group.
    -->
	<xsl:element name="xs:element">
		<xsl:attribute name="name" >
			<xsl:value-of select="concat(@name,'_Primitive')" />
		</xsl:attribute>
		<!-- TODO: support type with implicit schema -->
		<xsl:attribute name="type" >gml:AbstractTopoPrimitiveType</xsl:attribute>
		<xsl:attribute name="abstract" >true</xsl:attribute>
		<xsl:attribute name="substitutionGroup" >gml:_TopoPrimitive</xsl:attribute>
	</xsl:element>
	<!-- write the member property global element -->
	<xsl:element name="xs:element">
		<xsl:attribute name="name" >
			<xsl:value-of select="concat(@name,'_PrimitiveMember')" />
		</xsl:attribute>
		<!-- TODO: support type with implicit schema -->
		<xsl:attribute name="type" >
		    <xsl:value-of select="concat(ancestor::Schema/@name,':',@name,'_PrimitiveMemberType')" />
		</xsl:attribute>
		<xsl:attribute name="abstract" >false</xsl:attribute>
	</xsl:element>
	<!-- write the member property type -->
    <xsl:element name="xs:complexType">
		<xsl:attribute name="name" >
			<xsl:value-of select="concat(@name,'_PrimitiveMemberType')" />
		</xsl:attribute>
		<xsl:attribute name="abstract">false</xsl:attribute>
		<xsl:element name="xs:complexContent">
		    <xsl:element name="xs:restriction">
		        <xsl:attribute name="base">gml:topoPrimitiveMemberType</xsl:attribute>
		        <xsl:element name="xs:sequence">
		            <!-- reference the network primitve global element. This defines the allowable
		                 network primitive classes.
		            -->
		            <xsl:element name="xs:element">
		                <xsl:attribute name="ref">
		                    <xsl:value-of select="concat(ancestor::Schema/@name,':',@name,'_Primitive')" />
		                </xsl:attribute>
		            </xsl:element>
		        </xsl:element>
                <xsl:element name="xs:attributeGroup">
                    <xsl:attribute name="ref">gml:AssociationAttributeGroup</xsl:attribute>
                </xsl:element>
		    </xsl:element>
		</xsl:element>
    </xsl:element>
</xsl:template>

<!-- The following template acts on a network type class. It writes out any associations
     that this class might have.
-->
<xsl:template name="network_assocs_members">
    <xsl:param name="refFeatProp"/>
    <xsl:param name="parentPmtvProp"/>
    <xsl:param name="startNodeProp"/>
    <xsl:param name="endNodeProp"/>
    <xsl:param name="directedNodeProp"/>
    <xsl:variable name="referencerName" select="@name"/>
    
    <!-- Write the feature reference association -->
    <xsl:if test="not(string-length($refFeatProp) = 0)">
        <xsl:for-each select="ReferenceFeatureProperty/AssociationProperty">
            <xsl:call-template name="network_assoc_member">
                <xsl:with-param name="prefix">ReferenceFeature</xsl:with-param>
                <xsl:with-param name="referencerName" select="$referencerName"/>
                <xsl:with-param name="memberType">gml:FeaturePropertyType</xsl:with-param>
            </xsl:call-template>
        </xsl:for-each>
    </xsl:if>

    <!-- Write the parent network primitive association -->
    <xsl:if test="not(string-length($parentPmtvProp) = 0)">
        <xsl:for-each select="ParentNetworkProperty/AssociationProperty">
            <xsl:call-template name="network_assoc_member">
                <xsl:with-param name="prefix">ParentPrimitive</xsl:with-param>
                <xsl:with-param name="referencerName" select="$referencerName"/>
                <xsl:with-param name="memberType">gml:topoPrimitiveMemberType</xsl:with-param>
            </xsl:call-template>
        </xsl:for-each>
    </xsl:if>
    <!-- Write the start and end node (links only) associations -->
    <xsl:choose>
        <xsl:when test="not(string-length($startNodeProp) = 0) and ($startNodeProp = $directedNodeProp)">
            <!-- Just need a member property when start and end node classes are the same -->
            <xsl:for-each select="StartNode/AssociationProperty">
                <xsl:call-template name="network_assoc_member">
                    <xsl:with-param name="prefix">DirectedNode</xsl:with-param>
                    <xsl:with-param name="referencerName" select="$referencerName"/>
                    <xsl:with-param name="memberType">gml:topoPrimitiveMemberType</xsl:with-param>
                </xsl:call-template>
            </xsl:for-each>
        </xsl:when>
        <xsl:otherwise>
            <!-- Need a more complicated member property when start and end node classes are the different -->
            <xsl:call-template name="link_node_members">
                <xsl:with-param name="referencerName" select="$referencerName"/>
                <xsl:with-param name="startNodeProp" select="$startNodeProp" />
                <xsl:with-param name="endNodeProp" select="$endNodeProp" />
            </xsl:call-template>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<!-- The following writes out an association property for a network type class.
     The association is written as a GML property member.
 -->
<xsl:template name="network_assoc_member">
    <xsl:param name="prefix"/>
    <xsl:param name="referencerName"/>
    <xsl:param name="memberType"/>
    
    <!-- Write the global element for the property member -->
	<xsl:element name="xs:element">
		<xsl:attribute name="name" >
			<xsl:value-of select="concat($prefix,'_',$referencerName)" />
		</xsl:attribute>
		<!-- type is concatenation of name of current class and name of current association property -->
		<xsl:attribute name="type" >
		    <xsl:value-of select="concat(ancestor::Schema/@name,':',$referencerName,'_',@name)"/>
		</xsl:attribute>
		<xsl:attribute name="abstract" >false</xsl:attribute>
	</xsl:element>

    <!-- Write the type definition for the property member -->
    <xsl:element name="xs:complexType">
		<xsl:attribute name="name" >
			<xsl:value-of select="concat($referencerName,'_',@name)" />
		</xsl:attribute>
		<xsl:attribute name="abstract">false</xsl:attribute>
		<!-- handle any association property attributes -->
		<xsl:call-template name="association_attributes"/>
		<xsl:element name="xs:complexContent">
		    <xsl:element name="xs:restriction">
		        <xsl:attribute name="base"><xsl:value-of select="$memberType"/></xsl:attribute>
		        <xsl:element name="xs:sequence">
		            <!-- The member property references this assocation property's associated class -->
		            <xsl:element name="xs:element">
		                <xsl:attribute name="ref">
		                    <xsl:value-of select="concat(@associatedClassSchema,':',@associatedClass)" />
		                </xsl:attribute>
		            </xsl:element>
		        </xsl:element>
                <xsl:element name="xs:attributeGroup">
                    <xsl:attribute name="ref">gml:AssociationAttributeGroup</xsl:attribute>
                </xsl:element>
		    </xsl:element>
		</xsl:element>
    </xsl:element>   
</xsl:template>

<!-- The following writes the associations between the current link class and its associated
     start and end node classes. It is called only when the start and end node classes are different,
     since special handling is needed.
-->
<xsl:template name="link_node_members">
    <xsl:param name="referencerName"/>
    <xsl:param name="startNodeProp"/>
    <xsl:param name="endNodeProp"/>
    
    <xsl:if test="not(string-length($startNodeProp) = 0) or not(string-length($endNodeProp) = 0)">
        <!-- Write the global element for the associated node classes. -->
	    <xsl:element name="xs:element">
		    <xsl:attribute name="name" >
			    <xsl:value-of select="concat($referencerName,'_DirectedNodePrimitive')" />
		    </xsl:attribute>
		    <xsl:attribute name="abstract" >true</xsl:attribute>
	    </xsl:element>
	    <!-- Write the global element for the member property -->
	    <xsl:element name="xs:element">
		    <xsl:attribute name="name" >
			    <xsl:value-of select="concat('DirectedNode_',$referencerName)" />
		    </xsl:attribute>
		    <!-- TODO: support type with implicit schema -->
		    <xsl:attribute name="type" >
		        <xsl:value-of select="concat(ancestor::Schema/@name,':',$referencerName,'_DirectedNodeMemberType')"/>
		    </xsl:attribute>
		    <xsl:attribute name="abstract" >false</xsl:attribute>
	    </xsl:element>
	    <!-- Write the member property type -->
        <xsl:element name="xs:complexType">
		    <xsl:attribute name="name" >
			    <xsl:value-of select="concat($referencerName,'_DirectedNodeMemberType')" />
		    </xsl:attribute>
		    <xsl:attribute name="abstract">false</xsl:attribute>
		    <xsl:element name="xs:complexContent">
		        <xsl:element name="xs:restriction">
		            <xsl:attribute name="base">gml:topoPrimitiveMemberType</xsl:attribute>
		            <xsl:element name="xs:sequence">
		                <xsl:element name="xs:element">
		                    <!-- Member property referneces the global element for the associated 
		                         node classes.
		                    -->
		                    <xsl:attribute name="ref">
		                        <xsl:value-of select="concat(ancestor::Schema/@name,':',$referencerName,'_DirectedNodePrimitive')" />
		                    </xsl:attribute>
		                </xsl:element>
		            </xsl:element>
                    <xsl:element name="xs:attributeGroup">
                        <xsl:attribute name="ref">gml:AssociationAttributeGroup</xsl:attribute>
                    </xsl:element>
		        </xsl:element>
		    </xsl:element>
		</xsl:element>
		<!-- Write start node assocation as a global element with the above associated node 
		     class global element as its substitution group.
		-->
        <xsl:if test="not(string-length($startNodeProp) = 0)">
            <xsl:for-each select="StartNode/AssociationProperty">
                <xsl:call-template name="link_node_member">
                    <xsl:with-param name="referencerName" select="$referencerName"/>
                    <xsl:with-param name="nodeEnd">StartNode</xsl:with-param>
                </xsl:call-template>
            </xsl:for-each>
        </xsl:if>
		<!-- Write end node assocation as a global element with the above associated node 
		     class global element as its substitution group.
		-->
        <xsl:if test="not(string-length($endNodeProp) = 0)">
            <xsl:for-each select="EndNode/AssociationProperty">
                <xsl:call-template name="link_node_member">
                    <xsl:with-param name="referencerName" select="$referencerName"/>
                    <xsl:with-param name="nodeEnd">EndNode</xsl:with-param>
                </xsl:call-template>
            </xsl:for-each>
        </xsl:if>
    </xsl:if> 
</xsl:template>

<!-- Writes the association between a link class and its start or end node class -->
<xsl:template name="link_node_member">
    <xsl:param name="referencerName"/>
    <xsl:param name="nodeEnd"/>

    <!-- Association is represented by a global element -->
    <xsl:element name="xs:element">
        <xsl:attribute name="name" >
			    <xsl:value-of select="concat($referencerName,'_',@name)" />
		</xsl:attribute>
        <!-- The type is the node class -->
	    <xsl:attribute name="type" >
	        <xsl:call-template name="class_to_type">
                <xsl:with-param name="schema">
                    <xsl:call-template name="choose_schemaPrefix">
                        <xsl:with-param name="schema" select="@associatedClassSchema" />
                        <xsl:with-param name="prefix" select="//Schema[@name=@associatedClassSchema]/@prefix" />
                    </xsl:call-template>
                </xsl:with-param>
	            <xsl:with-param name="class" select="@associatedClass"/>
	        </xsl:call-template>
	    </xsl:attribute>
        <!-- The substitution group is the associated node property for the referencing link class -->
	    <xsl:attribute name="substitutionGroup" >
	        <xsl:value-of select="concat(@associatedClassSchema,':',$referencerName,'_DirectedNodePrimitive')"/>
	    </xsl:attribute>
	    <xsl:attribute name="abstract" >false</xsl:attribute>
	    <!-- refType records whether this association is for start or end node -->
	    <xsl:attribute name="fdo:refType"><xsl:value-of select="$nodeEnd"/></xsl:attribute>
	    <!-- Write the association property attributes -->
		<xsl:call-template name="association_attributes"/>
	</xsl:element>
</xsl:template>

<!-- For each association property that references an associated class in a different
 schema, write out an "external" global element, to provide the link to the 
 associated class. This element is needed since the associated class might not be
 in this XML document.
-->
<xsl:template name="external_elements">
    <xsl:variable name="schemaName" select="@name" />
    <!-- Generate external nodes for association properties -->
    <xsl:for-each select="node()/Properties/AssociationProperty">
        <!-- Skip properties that reference associated class in the same schema -->
        <xsl:if test="not($schemaName = @associatedClassSchema)">
	        <xsl:variable name="elemName" select="concat('AssociatedClass','_',../../@name,'_',@name)" />
	        <!-- Generate an xs:key (primary key) or xs:unique (alternate key) depending
	         on whether identity properties are implicit.
	        -->
	        <xsl:variable name="keyType">
	            <xsl:choose>
	                <xsl:when test="IdentityProperties/@default">
						<!-- GML type association written so this external element does not need a key -->
	                    <xsl:value-of select="''"/>
	                </xsl:when>
	                <xsl:otherwise>
						<!-- XML schema keyref  type association written so unique key required -->
	                    <xsl:value-of select="'xs:unique'"/>
	                </xsl:otherwise>
	            </xsl:choose>
	        </xsl:variable>
	        <xsl:element name="xs:element" >
		        <xsl:attribute name="name" >
			        <xsl:value-of select="$elemName" />
		        </xsl:attribute>
		        <xsl:attribute name="type" >
		            <xsl:call-template name="class_to_type">
                        <xsl:with-param name="schema">
                            <xsl:call-template name="choose_schemaPrefix">
                                <xsl:with-param name="schema" select="@associatedClassSchema" />
                                <xsl:with-param name="prefix" select="//Schema[@name=@associatedClassSchema]/@prefix" />
                            </xsl:call-template>
                        </xsl:with-param>
		                <xsl:with-param name="class" select="@associatedClass"/>
		            </xsl:call-template>
		        </xsl:attribute>
		        <!-- External element always abstract. It's sole purpose is to 
		         support association properties.
		        -->
		        <xsl:attribute name="abstract" >
			        <xsl:value-of select="'true'" />
		        </xsl:attribute>
		        <xsl:if test="not(string-length($keyType) = 0)">
					<xsl:for-each select="IdentityProperties" >
						<!-- Identity properties kept in a key -->
						<xsl:element name="{$keyType}" >
							<xsl:attribute name="name" >
								<xsl:value-of select="concat($elemName,'Key')" />
							</xsl:attribute>
							<xsl:element name="xs:selector" >
								<xsl:attribute name="xpath" >
									<xsl:value-of select="concat('.//',../@name)" />
								</xsl:attribute>
							</xsl:element>
							<xsl:for-each select="IdentityProperty" >
								<xsl:element name="xs:field" >
									<xsl:attribute name="xpath" >
										<xsl:value-of select="." />
									</xsl:attribute>
								</xsl:element>
							</xsl:for-each>
						</xsl:element>
					</xsl:for-each>
		        </xsl:if>
		    </xsl:element>
		</xsl:if>
    </xsl:for-each>
</xsl:template>

<!-- This template writes attributes common to all network primitive class -->
<xsl:template name="network_pmtv_attributes">
	<xsl:call-template name="feature_class_attributes" />
	<xsl:if test="@costProperty" >
		<xsl:attribute name="fdo:costElement" >
			<xsl:value-of select="@costProperty" />
		</xsl:attribute>
	</xsl:if>
</xsl:template>

<!-- This templates writes attributes for feature classes -->
<xsl:template name="feature_class_attributes">
	<xsl:call-template name="class_attributes" />
	<xsl:if test="@hasGeometry" >
		<xsl:attribute name="fdo:hasGeometry" >
			<xsl:value-of select="@hasGeometry" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="@geometryProperty" >
		<xsl:attribute name="fdo:geometryName" >
			<xsl:value-of select="@geometryProperty" />
		</xsl:attribute>
		<xsl:variable name="geometryProperty" select="@geometryProperty" />
		<!-- mark inherited geometry properties so we don't create a 
		     duplicate geometric property when reading from XML.
		-->
		<xsl:if test="not(Properties/GeometricProperty[@name=$geometryProperty])" >
			<xsl:attribute name="fdo:geometryInherited" >true</xsl:attribute>
		</xsl:if>
	</xsl:if>
</xsl:template>

<!-- This templates writes attributes for any FDO class definitions -->
<xsl:template name="class_attributes">
	<xsl:attribute name="name" >
	    <!-- Append "Type" suffix to convert class name to type name -->
		<xsl:value-of select="concat(@name,'Type')" />
	</xsl:attribute>
	<xsl:attribute name="abstract" >
		<xsl:value-of select="@abstract" />
	</xsl:attribute>
</xsl:template>

<!-- This templates writes attributes for assocation properties -->
<xsl:template name="association_attributes">
	<xsl:if test="@multiplicity and not(@multiplicity='m')" >
		<xsl:attribute name="fdo:multiplicity" >
			<xsl:value-of select="@multiplicity" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="@deleteRule" >
		<xsl:attribute name="fdo:deleteRule" >
			<xsl:value-of select="@deleteRule" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="@lockCascade" >
		<xsl:attribute name="fdo:lockCascade" >
			<xsl:value-of select="@lockCascade" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="@reverseName" >
		<xsl:attribute name="fdo:reverseName" >
			<xsl:value-of select="@reverseName" />
		</xsl:attribute>
	</xsl:if>
	<xsl:if test="@readOnly" >
		<xsl:attribute name="fdo:readOnly" >
			<xsl:value-of select="@readOnly" />
		</xsl:attribute>
	</xsl:if>
</xsl:template>

<!-- This template writes element restrictions common to all network primitive classes -->
<xsl:template name="network_primitive_restrict" >
    <xsl:call-template name="network_restrict" />
    <xsl:element name="xs:element">
        <xsl:attribute name="ref">gml:isolated</xsl:attribute>
        <xsl:attribute name="minOccurs">0</xsl:attribute>
        <xsl:attribute name="maxOccurs">0</xsl:attribute>
    </xsl:element>
    <xsl:element name="xs:element">
        <xsl:attribute name="ref">gml:container</xsl:attribute>
        <xsl:attribute name="minOccurs">0</xsl:attribute>
        <xsl:attribute name="maxOccurs">0</xsl:attribute>
    </xsl:element>
</xsl:template>

<!-- This template writes element restrictions common to all network type classes -->
<xsl:template name="network_restrict" >
    <xsl:element name="xs:element">
        <xsl:attribute name="ref">gml:metaDataProperty</xsl:attribute>
        <xsl:attribute name="minOccurs">0</xsl:attribute>
        <xsl:attribute name="maxOccurs">0</xsl:attribute>
    </xsl:element>
    <xsl:element name="xs:element">
        <xsl:attribute name="ref">gml:description</xsl:attribute>
        <xsl:attribute name="minOccurs">0</xsl:attribute>
        <xsl:attribute name="maxOccurs">0</xsl:attribute>
    </xsl:element>
    <xsl:element name="xs:element">
        <xsl:attribute name="ref">gml:name</xsl:attribute>
        <xsl:attribute name="minOccurs">0</xsl:attribute>
        <xsl:attribute name="maxOccurs">0</xsl:attribute>
    </xsl:element>
</xsl:template>

<!-- This template writes all class properties -->
<xsl:template name="class_properties">
	<xsl:for-each select="Properties[not(count(node())= 0)]" >
		<xsl:element name="xs:sequence">
			<xsl:apply-templates select="*" />
		</xsl:element>
	</xsl:for-each>
</xsl:template>

<!-- Write string data property as an xs:string with restriction on length -->
<xsl:template name="dataproperty_string">
	<xsl:call-template name="element_subelements" />
	<xsl:element name="xs:simpleType">
		<xsl:element name="xs:restriction">
		    <xsl:attribute name="base">xs:string</xsl:attribute>
            <xsl:if test="@length and @length != ''">
                <xsl:element name="xs:maxLength">
                    <xsl:attribute name="value" >
                        <xsl:value-of select="@length" />
                    </xsl:attribute>
                </xsl:element>
            </xsl:if>
      <xsl:apply-templates select="Constraint" />
		</xsl:element>
	</xsl:element>
</xsl:template>

<!-- Write decimal data property as an xs:decimal with restriction on scale and precision -->
<xsl:template name="dataproperty_decimal">
	<xsl:call-template name="element_subelements" />
	<xsl:element name="xs:simpleType">
		<xsl:element name="xs:restriction">
		    <xsl:attribute name="base">xs:decimal</xsl:attribute>
			<xsl:element name="xs:totalDigits">
				<xsl:attribute name="value" >
					<xsl:value-of select="@precision" />
				</xsl:attribute>
			</xsl:element>
			<xsl:element name="xs:fractionDigits">
			    <xsl:choose>
			        <xsl:when test="@scale &lt; 0">
			            <!-- Xml Schema doesn't allow negative scale, so
			                 set scale to 0 (the best we can do) and store
			                 the scale in an FDO attribute.
			            -->
        				<xsl:attribute name="value" >0</xsl:attribute>
				        <xsl:attribute name="fdo:scale" >
					        <xsl:value-of select="@scale" />
				        </xsl:attribute>
			        </xsl:when>
			        <xsl:otherwise>
				        <xsl:attribute name="value" >
					        <xsl:value-of select="@scale" />
				        </xsl:attribute>
			        </xsl:otherwise>
			    </xsl:choose>
			</xsl:element>
      <xsl:apply-templates select="Constraint" />
    </xsl:element>
	</xsl:element>
</xsl:template>

<!-- For other types of data properties, just set the type -->
<xsl:template name="dataproperty_other">
    <xsl:param name="gmlType" />
    <xsl:element name="xs:simpleType">
        <xsl:element name="xs:restriction">
            <xsl:attribute name="base">
                <xsl:value-of select="$gmlType" />
            </xsl:attribute>
            <xsl:apply-templates select="Constraint" />
        </xsl:element>
    </xsl:element>
    <xsl:call-template name="element_subelements" />
</xsl:template>

<!-- For the dataproperty's list and range restriction  -->
<xsl:template match="Constraint">
  <xsl:choose>
    <xsl:when test="@type = 'list'" >
      <xsl:call-template name="restriction_list" />
    </xsl:when>
    <xsl:when test="@type = 'range'" >
      <xsl:call-template name="restriction_range" />
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="restriction_list">
  <xsl:apply-templates select="Values" />
</xsl:template>
  
<xsl:template match="Values">
  <xsl:for-each select="Value">
    <xsl:element name="xs:enumeration">
      <xsl:attribute name="value">
        <xsl:value-of select="."/>
      </xsl:attribute>
    </xsl:element>
  </xsl:for-each>
</xsl:template>

<xsl:template name="restriction_range">
  <xsl:apply-templates select="Max" />
  <xsl:apply-templates select="Min" />
</xsl:template>

<xsl:template match="Max">
  <xsl:choose>
    <xsl:when test="@inclusive = 'true'" >
      <xsl:element name="xs:maxInclusive">
        <xsl:attribute name="value">
          <xsl:value-of select="@value"/>
        </xsl:attribute>
      </xsl:element>
    </xsl:when>
    <xsl:when test="@inclusive = 'false'" >
      <xsl:element name="xs:maxExclusive">
        <xsl:attribute name="value">
          <xsl:value-of select="@value"/>
        </xsl:attribute>
      </xsl:element>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template match="Min">
  <xsl:choose>
    <xsl:when test="@inclusive = 'true'" >
      <xsl:element name="xs:minInclusive">
        <xsl:attribute name="value">
          <xsl:value-of select="@value"/>
        </xsl:attribute>
      </xsl:element>
    </xsl:when>
    <xsl:when test="@inclusive = 'false'" >
      <xsl:element name="xs:minExclusive">
        <xsl:attribute name="value">
          <xsl:value-of select="@value"/>
        </xsl:attribute>
      </xsl:element>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<!-- Write geometric property attributes -->
<xsl:template name="geometricproperty_attributes">
	<xsl:attribute name="fdo:hasMeasure" >
		<xsl:value-of select="@hasMeasure" />
	</xsl:attribute>
	<xsl:attribute name="fdo:hasElevation" >
		<xsl:value-of select="@hasElevation" />
	</xsl:attribute>
	<xsl:if test="@associatedSCName">
	    <xsl:attribute name="fdo:srsName" >
		    <xsl:value-of select="@associatedSCName" />
	    </xsl:attribute>
	</xsl:if>
	<xsl:attribute name="fdo:geometricTypes" >
		<xsl:for-each select="GeometricTypes/GeometricType">
			<xsl:value-of select="concat(.,' ')" />
		</xsl:for-each>
	</xsl:attribute>
    <xsl:attribute name="fdo:geometryTypes" >
        <xsl:for-each select="GeometryTypes/GeometryType">
            <xsl:value-of select="concat(.,' ')" />
        </xsl:for-each>
    </xsl:attribute>
</xsl:template>

<!-- Generates the name of the key referenced by an association property's
 keyref. 
-->
<xsl:template name="assoc_property_key">
    <xsl:param name="includeSchema">true</xsl:param>
    <xsl:variable name="schemaPrefix">
        <xsl:if test="$includeSchema='true'">
            <xsl:value-of select="concat(ancestor::Schema/@name,':')"/>
        </xsl:if>
    </xsl:variable>
    <xsl:choose>
        <!-- When association property and associated class schemas are different,
         the reference is to a key in an external global element
        -->         
        <xsl:when test="not(ancestor::Schema/@name = @associatedClassSchema)" >
            <xsl:value-of select="concat($schemaPrefix,'AssociatedClass','_',../../@name,'_',@name,'Key')" />
        </xsl:when>
        <!-- Otherwise, if identity properties are explicit, then it is to 
         a global element's xs:key
        -->
        <xsl:when test="IdentityProperties/@default='true'" >
            <xsl:value-of select="concat($schemaPrefix,@associatedClass,'Key')" />
        </xsl:when>
        <!-- Otherwise, the referenced key name contains the reverse name -->
        <xsl:when test="../@reverseName">
            <xsl:value-of select="concat($schemaPrefix,@associatedClass,'_',@reverseName,'AKey')" />
        </xsl:when>
        <!-- Otherwise, the referenced key name is a concatenation of the associated class
         and the association property name -->
        <xsl:otherwise>
            <xsl:value-of select="concat($schemaPrefix,@associatedClass,'_',../../@name,'AKey')" />
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<!-- Generates the name of the element referenced by a GML type assocation.
-->
<xsl:template name="assoc_property_ref">
    <xsl:param name="includeSchema">true</xsl:param>
    <xsl:variable name="schemaPrefix">
        <xsl:if test="$includeSchema='true'">
            <xsl:value-of select="concat(ancestor::Schema/@name,':')"/>
        </xsl:if>
    </xsl:variable>
    <xsl:choose>
        <!-- When association property and associated class schemas are different,
         the reference is to an external global element
        -->         
        <xsl:when test="not(ancestor::Schema/@name = @associatedClassSchema)" >
            <xsl:value-of select="concat($schemaPrefix,'AssociatedClass','_',../../@name,'_',@name)" />
        </xsl:when>
        <!-- Otherwise, it is to a regular global element in the same schema.
        -->
        <xsl:otherwise >
            <xsl:value-of select="concat($schemaPrefix,@associatedClass)" />
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<!-- Write attributes common to all fdo elements -->
<xsl:template name="element_attributes">
	<xsl:attribute name="name" >
		<xsl:value-of select="@name" />
	</xsl:attribute>
</xsl:template>
		
<!-- Write sub-elements common to all fdo elements -->
<xsl:template name="element_subelements">
	<xsl:if test="Description|SAD|@DataModelType|@DataType|@Organization|@TileXSize|@TileYSize" >
		<xsl:element name="xs:annotation">
		    <xsl:call-template name="annotation_subelements" />
		</xsl:element>
	</xsl:if>
</xsl:template>

<xsl:template name="annotation_subelements">
    <!-- Write the description -->
    <xsl:for-each select="Description" >
        <xsl:element name="xs:documentation">
	        <xsl:value-of select="." />
        </xsl:element>
    </xsl:for-each>
   <!-- write the SAD and raster info as appinfo -->
   	<xsl:if test="SAD|@DataModelType|@DataType|@Organization|@TileXSize|@TileYSize" >
	    <xsl:element name="xs:appinfo" >
	        <xsl:attribute name="source">http://fdo.osgeo.org/schemas</xsl:attribute>
	        <!-- Write the Schema Attribute Dictionary -->
		    <xsl:for-each select="SAD" >
    	        <xsl:for-each select="SADItem">
			        <xsl:element name="fdo:attribute" >
				        <xsl:attribute name="name" >
					        <xsl:value-of select="@name" />
				        </xsl:attribute>
				        <xsl:value-of select="." />
			        </xsl:element>						
		        </xsl:for-each>
		    </xsl:for-each>
            <xsl:if test="@DataModelType|@DataType|@Organization|@TileXSize|@TileYSize" >
	            <!-- Write the default data model for raster properties -->
		        <xsl:element name="fdo:DefaultDataModel" >
		            <xsl:if test="@DataModelType" >
		                <xsl:attribute name="dataModelType" >
			                <xsl:value-of select="@DataModelType" />
		                </xsl:attribute>
    		        </xsl:if>
                    <xsl:if test="@DataType" >
                        <xsl:attribute name="dataType" >
                            <xsl:value-of select="@DataType" />
                        </xsl:attribute>
                    </xsl:if>
                    <xsl:if test="@Organization" >
		                <xsl:attribute name="organization" >
			                <xsl:value-of select="@Organization" />
		                </xsl:attribute>
    		        </xsl:if>
	    	        <xsl:if test="@BitsPerPixel" >
		                <xsl:attribute name="bitsPerPixel" >
			                <xsl:value-of select="@BitsPerPixel" />
		                </xsl:attribute>
		            </xsl:if>
		            <xsl:if test="@TileSizeX" >
		                <xsl:attribute name="tileSizeX" >
			                <xsl:value-of select="@TileSizeX" />
    		            </xsl:attribute>
	    	        </xsl:if>
		            <xsl:if test="@TileSizeY" >
		                <xsl:attribute name="tileSizeY" >
			                <xsl:value-of select="@TileSizeY" />
		                </xsl:attribute>
    		        </xsl:if>
	            </xsl:element>
	        </xsl:if>
        </xsl:element>						
	</xsl:if>
</xsl:template>

<!-- Specialized template that takes the base class info for the 
     current element and converts it to a GML type name
-->  
<xsl:template name="base_class_to_type">
    <xsl:call-template name="class_to_type">
        <xsl:with-param name="schema">
            <xsl:call-template name="choose_schemaPrefix">
                <xsl:with-param name="schema" select="@baseSchema" />
                <xsl:with-param name="prefix" select="//Schema[@name=@baseSchema]/@prefix" />
            </xsl:call-template>
        </xsl:with-param>
        <xsl:with-param name="class" select="@baseClass"/>
    </xsl:call-template>
</xsl:template>

<!-- Converts qualified class name to GML Type name by adding
     a "Type" suffix.
-->
<xsl:template name="class_to_type">
    <xsl:param name="schema"/>
    <xsl:param name="class"/>

    <xsl:variable name="prefix">
        <xsl:call-template name="schema_to_prefix">
            <xsl:with-param name="schema" select="$schema"/>
        </xsl:call-template>
    </xsl:variable>

	<xsl:value-of select="concat($prefix,':',$class,'Type')"/>
</xsl:template>

<!-- Converts FDO Feature Schema name to its target namespace URI.
-->
<xsl:template name="schema_to_uri">
    <xsl:param name="schema"/>

    <xsl:variable name="decodedSchema">
        <xsl:call-template name="decode_schema">
            <xsl:with-param name="schema" select="$schema"/>
        </xsl:call-template>
    </xsl:variable>
   	<xsl:value-of select="concat($q_customer_url,$decodedSchema)"/>
</xsl:template>

<!-- Converts encoded '/' characters back to '/'-->
<xsl:template name="decode_schema">
    <xsl:param name="schema"/>
    <xsl:choose>
        <xsl:when test="contains($schema,'-x2f-')">
            <xsl:variable name="leftPart" select="substring-before($schema,'-x2f-')"/>
            <xsl:variable name="rightPart" select="substring-after($schema,'-x2f-')"/>
            <xsl:variable name="decodedRightPart">
                <xsl:call-template name="decode_schema">
                    <xsl:with-param name="schema" select="$rightPart"/>
                </xsl:call-template>
            </xsl:variable>
            <xsl:value-of select="concat($leftPart,'/',$decodedRightPart)"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="$schema"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<!-- Converts FDO Feature Schema name to its namespace prefix.
-->
<xsl:template name="schema_to_prefix">
    <xsl:param name="schema"/>

    <xsl:variable name="rightPart">
        <xsl:choose>
            <xsl:when test="contains($schema,'/')">
                <xsl:call-template name="schema_to_prefix">
                    <xsl:with-param name="schema" select="substring-after($schema,'/')"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:when test="contains($schema,'-x2f-')">
                <xsl:call-template name="schema_to_prefix">
                    <xsl:with-param name="schema" select="substring-after($schema,'-x2f-')"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$schema"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>
    
    <xsl:choose>
        <xsl:when test="$rightPart=''">
            <xsl:value-of select="$schema"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="$rightPart"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<!-- Choose FDO Schema target namespace prefix.-->
<xsl:template name="choose_schemaPrefix">
    <xsl:param name="schema"/>
    <xsl:param name="prefix"/>
        <xsl:choose>
            <xsl:when test="$prefix | $prefix !=''">
                <xsl:value-of select="$prefix"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$schema"/>
            </xsl:otherwise>
        </xsl:choose>
</xsl:template>

<!-- Choose FDO Schema target namespace.-->
<xsl:template name="choose_targetNamespace">
    <xsl:param name="schema"/>
    <xsl:param name="custom_targetNamespace"/>
    <xsl:choose>
        <xsl:when test="$custom_targetNamespace | $custom_targetNamespace !=''">
            <xsl:value-of select="$custom_targetNamespace"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:call-template name="schema_to_uri">
                <xsl:with-param name="schema" select="$schema"/>
            </xsl:call-template>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

</stylesheet>


