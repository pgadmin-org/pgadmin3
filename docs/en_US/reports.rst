.. _reports:


********************
`Report Tool`:index:
********************

pgAdmin includes a simple reporting engine which allows you to quickly
generate reports from the queries you write in the :ref:`Query Tool <query>`,
or from objects or collections of objects in the main
application Window - for example, you can create a report of the properties
of any object, or a list of functions in a schema. To create a report,
select a node in the browser treeview, and select the report to create from
the context menu, or from the Reports submenu of the Tools menu. To create
a report in the Query Tool, select the Quick Report option from the File menu.

.. image:: images/reporttool-html.png

Each report contains a title and optional notes that can be modified before
the report is produced. In addition, you may select whether or not to include
any SQL that may be relevant to the report you have selected.

Reports are generated internally in XML, however pgAdmin can apply an XML
stylesheet to the report at processing time to format the output as required. A
default stylesheet is built-in which produces HTML output. With the HTML output
option selected, you can opt to embed the default CSS stylesheet (which will 
render the report in the same colors as the pgAdmin website), to embed an external
stylesheet into the report, or to link to an external stylesheet. The following 
class/object ID's are used:

* **#ReportHeader**: This div contains the report header.
* **#ReportNotes**: This div contains the option report notes.
* **#ReportDetails**: This div contains the main body of the report.
* **#ReportFooter**: This div contains the report footer.
* **.ReportSQL**: This class is used by the <PRE></PRE> blocks containing SQL.
* **.ReportDetailsOddDataRow**: This class is applied to the odd numbered rows of tables.
* **.ReportDetailsEvenDataRow**: This class is applied to the even numbered rows of tables.
* **.ReportTableHeaderCell**: This class is applied to table header cells.
* **.ReportTableValueCell**: This class is applied to table data cells.
* **.ReportTableInfo**: This class is applied to table footnotes.

.. image:: images/reporttool-xml.png

When generating reports in XML format, you can opt to output plain XML,
XML linked to an external XSL stylesheet, or to process the XML using an 
external stylesheet and save the resulting output. This allows complete 
flexibility to format reports in any way.

The default :ref:`XSL stylesheet <default-xsl>` used to render 
XHTML output can be used as a starting point for your own, and sample 
:ref:`XML data <sample-xml>` may also be reviewed if required.

Contents:

.. toctree::
   :maxdepth: 2

   default-xsl