.. _sample-xml:


************************
`Sample XML Data`:index:
************************

The sample report below demonstrates the XML format that the
pgadmin :ref:`Report Tool <reports>` will produce.

::

    <?xml version="1.0" encoding="UTF-8"?>
    <?xml-stylesheet type="text/xsl" href="C:\stylesheet.xsl" ?>

    <report>

      <header>

        <!-- All the values in the header section are optional, apart from the title. -->

        <title>Sample XML file</title>
        <notes>This is a sample XML file.</notes>
        <generated>10/05/2006 11:21:23</generated>
        <server>localhost:5432</database>
        <database>postgres</database>
        <schema>public</schema>
        <table>pg_ts_cfg</table>
        <job></job>
      </header>

      <!-- Multiple report sections may be present, and must have a name -->

      <section id="s1" number="1" name="Query results">

        <!-- A table is optional within a section. If present, the number -->
        <!-- of columns should be appropriate for the attributes specified -->
        <!-- in each row. A row needn't specify every value however. -->

        <table>
          <columns>
            <column id="c1" number="1" name="oid" />
            <column id="c2" number="2" name="ts_name" />
            <column id="c3" number="3" name="prs_name" />
            <column id="c4" number="4" name="locale" />
          </columns>
          <rows>
            <row id="r1" number="1" c1="17108" c2="default" c3="default" c4="C" />
            <row id="r2" number="2" c1="17109" c2="default_russian" c3="default" c4="ru_RU.KOI8-R" />
            <row id="r3" number="3" c1="17110" c2="simple" c3="default" c4="" />
          </rows>

          <!-- Additional section specific info may be supplied. -->

          <info>3 rows with 4 columns retrieved.</info>

        </table>

        <!-- Each section may also contain some related SQL. -->

        <sql>SELECT oid, * FROM pg_ts_cfg</sql>

      </section>

    </report>

