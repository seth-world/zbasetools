#ifndef ZXMLDOC_H
#define ZXMLDOC_H
/**
  ZXML : Thin libxml2 wrapper

  DTD are not managed here.
  Encryptions and conversions are not managed here for a dynamic usage,
  but an xml document, encrypted in other format than UTF8 will be parsed as soon at its encryption is recognized by libxml2,
  and a DOM document may be created and saved under a defined encryption, recognized by libxml2.

  If you do need more complexity, use QT XML, or libxml++

  Back to the basics :

  - a node is any elementary piece of xml : text, attribute, document,...

  - an element is a node that may have
    - a name
    - children (text node(s) and/or comment(s) and/or attribute(s) and/or document fragment(s)...

  each DOM document has one and only one Root element.
  This element holds the whole tree.
  It may itself have comments, CData sections, text, attribute

  During DOM document creation, this root element may be replaced, by a new one, or a set of nodes organized in a tree.
  Setting/replacing the document's root element by the top node of an xml tree will do this.

  - a CData section is a node which content is not interpreted as XML text.
  Content of a CData section is left to itself without interpretation, without entities substitution, etc...

  A CData section concerns only nodes and not attributes. An attribute cannot be or have a CData section as child,
  excepted if defined in Document's DTD.
  DTD is not managed with this thin client.

  - namespace : a namespace is a special node that holds an url and a href that will prefix tree element's name.

  a namespace may be global, at DOM document's level, or local, defined for one element




  zxmlNode creation / deletion

  zxmlNodes objects are created, and therefore used, as pointers using zxmlcreateNode() generic function (and exclusively with that function).
  It return a pointer to the newly created node that holds the underlying element/node data.

  Nodes MUST NOT be deleted, excepted using zxmlremoveNode() generic function.

  The mechanism is the same for namespace objects : zxmlNameSpace, using zxmlcreateNameSpace() and zxmlremoveNameSpace(),
  as well as attributes with zxmlcreateAttribute() and zxmlremoveAttribute().

  Not using this mechanism will induce possible memory leaks / double free corruption.


  setXMLZException() :

  sets ZException with message and complements with xml error if any available.



Creating
--------
xmlNewDoc
xmlNewDocFragment

xmlSaveFile
xmlSaveFileEnc
xmlSaveFormatFile
xmlSaveFormatFileEnc


xmlDocSetRootElement

xmlNewCDataBlock

xmlNewChild

xmlNewComment
xmlNewDocComment

xmlNewDocNode
xmlNewDocRawNode
xmlNewNode
xmlNewDocText
xmlNewDocTextLen

xmlNewText
xmlNewTextLen
xmlNewTextChild

xmlNodeSetName


xmlNodeSetContent
xmlNodeSetContentLen

xmlNodeAddContent
xmlNodeAddContentLen		(


xmlNewDocProp
xmlNewProp
xmlSetProp

xmlNodeSetLang

xmlSetTreeDoc	 update all nodes under tree node to point to right document

xmlNewReference



Parsing
-------
xmlDocGetRootElement

xmlFirstElementChild

xmlLastElementChild

xmlNextElementSibling
xmlPreviousElementSibling

xmlNodeGetContent

xmlNodeGetLang

xmlGetProp
xmlHasProp

xmlIsBlankNode
xmlIsXHTML



*/
#endif // ZXMLDOC_H
