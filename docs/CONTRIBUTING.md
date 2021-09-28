# Contributing to this repository <!-- omit in toc -->

## Getting started <!-- omit in toc -->

Before you begin:
- Have you read the [code of conduct](CODE_OF_CONDUCT.md)?
- Check out the [existing issues](https://github.com/TadPath/PUMA/issues) 


## Language

All documentation and source code comments are to be written using UK / British English and spellings.

## Computer code

Please keep to the coding and comment style used in the original source files.
Please ensure alignment of any comment delimeters is preserved after editing any comments
The preferred language is C or Python. Other acceptable languages are those that may be compiled with open source compilation tools such as gcc, g++ or similar.
Java and Java script are not permitted.
Any scripting language embedded in Microsoft Word, Excel or other MS Office product will not be allowed as part of the PUMA open source standard.

## Hardware

The general rules for anyone wanting to contribute hardware specs, corrections, new modules design and methods are:
* Only complete CAD files readable with FreeCAD are accepted which include the editable models but NOT meshes.
* STL and 3MF (or other mesh-type) files are not permitted.
* The models must be 3D printable on a low-to-medium end FDM 3D printer (such as the Creality Ender 3).
* Any post processing required to assemble or fit the part must not use specialist tools beyond those commonly and cheaply available to most people without specialist machine working skills. In particular only the following items are acceptable: Craft knife (like a Stanley knife), hand held tools like screwdriver, spanner, hammer, hand-held electric drill and common houshold objects like a coin or aluminium foil.
* Any design requiring specialist power tools such as a pillar drill, a lathe, a CNC router, etc. will not be acceptable.
* Any non-printed items must be easily available from muiltiple suppliers e.g. on AliExpress, eBay, Amazon or a general hardware store.
* No module requiring special one-off components - even if available from the above-mentioned stores - shall be acceptable. By 'special one-off' I mean such things as specially made modules designed to fit in products that are no longer sold (end of line). An example would be a particular brand of LCD projector or the motorisation assembly for a particular camera focus mechanism. Such things may be bought cheaply and provide excellent functionality but they do not pass the test of longevity and generic availability. If PUMA were to use such modules for  it's parts, those parts have an undue risk of becoming obsolete.
* Hardware modules designed to interface to vendor-specific hardware may be considered if generic adaptor editability is built into the module CAD design. For example a generic smartphone eyepiece adapter module with an add-on part that allows any one of a range of vendor specific smartphones to be used would be fine.

Of course there is nothing to stop anyone using any of the above to make their own customisations for their own needs or forks of PUMA. However these are the rules for anyone wanting to contribule their model as part of the official PUMA open source standard package.

## Electronics
* Use of Arduino and Raspberry Pi-based solutions are welcome provided any additional interface board conforms tot he following:
  * It is already widely commercially available from common stores such as those metioned above.
  * If not already widely available it can be made with moderate ease using tripad or veroboard strip pads (pre-drilled copper-clad boards).
  * Any interface or additional PCB required which can be made from the above pre-drilled boards may also have a custom PCB version submitted provided that custom PCB and the self-built pre-drilled boards are cross compatible in terms of all functionality and physical fit to a 3D printed enclosure.
  * Any custom PCB that does not conform to the above will not be acceptable as part of the PUMA standard.
  * All components used must be easily avilable from stores such as RS, Farnell CPC, Element 14 or AliExpress.
  * All electronics must be supplied with full details of components and construction.
  * Any enclosure must be 3D printable and the full CAD files supplied and subject to the rules mentioned above for Hardware.
  * Any electronics must also have a detailed end user guide submitted as well in order to be acceptable as part of the open source PUMA standard.
  * All associated software must also be submitted in open source GPL v3 to be compatible with the license of the PUMA project.

## Images, Design Files and Other Copyright Matters
Any images, design files, music, logos or other content subject to possible design rights and copyright used in any contribution, regardless of type, must have copyright or other intellectual property rights owned by the contributer(s) or be under a type of license that allows the contributors to use those materials in any documents or models they submit and allow them also to be used under the GPL license used by the PUMA repositorty. Any attributions required for such use must be made correctly by the contributor in their submission.

## Logos, Branding and Trade Marks
No trade marks or brand logos are to be part of any submitted materials (apart from those present on components that need to be purchased).

