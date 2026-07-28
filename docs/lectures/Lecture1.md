# The Foundations of Requirements 
- Requirements discovery and where requirements come from. 


## Requirement Engineering
The requirements for a system are the descriptions of the services that a system should provide and the constraints on its operation. The process of finding out, analyzing, documenting and checking these services and constraints is called requirements engineering (RE).  
The term requirement is not used consistently in the software industry.  
For example,  If I were to raise a contract, I might not specify exactly what I need the system to do. This leaves room for interpretation and the vendor can envision different ways to fulfill the contract.


## User requirements
User communicate their needs in the form of natural language + some diagrams. This primarily points at what the user wants the system to do.
They usually dont specify the technical details of the system.

## System requirements
System requirements are the formalized, specific, and measurable requirements that define what the system must do. For a given requirement, the system must fulfill the requirement.


## Mapping user requirements to system requirements
|User Requirement|System Requirement|
|---|---|
|1. The Mentcare system shall generate monthly management reports showing the cost of drugs prescribed by each clinic during that month.|1.1 On the last working day of each month, a summary of the drugs prescribed, their cost and the prescribing clinics shall be generated.|
||1.2 The system shall generate the report for printing after 17:30 on the last working day of the month.
||1.3 A report shall be created for each clinic and shall list the individual drug names, the total number of prescriptions, the number of doses prescribed and the total cost of the prescribed drugs.
||1.4 If drugs are available in different dose units (e.g. 10mg, 20mg, etc.) separate reports shall be created for each dose unit.
||1.5 Access to drug cost reports shall be restricted to authorized users as listed on a management access control list.|

## System stakeholders
Anyone who is genuinely interested in the system's functionality and is involved in its development and use can be considered a stakeholder.  

|User requirement|System requirement|
|---|---|
|Client managers|System end-users|
|System end-users|Client engineers|
|Client engineers|System architects|
|Contractor managers|Software developers
|System architects||

## Feasibility studies
A feasibility study is a short, focused study that should take place early in the RE process. It should answer three
key questions:   
1. Does the system contribute to the overall objectives of the organization?  
2. Can the system be implemented within schedule and budget using current technology?  
3. Can the system be integrated with other systems that are used?  

**In-class exercise:** six institute-wide system proposals that sound attractive but fail one or more of these questions. Each is a flash card — the pitch on one slide, the feasibility verdict on the next.

```slides
title: Feasibility flash cards
url_stub: lecture1-feasibility
nav:
  - lecture1_slides/01_title.md
  - lecture1_slides/02_three_questions.md
  - lecture1_slides/03_p1_pitch.md
  - lecture1_slides/04_p1_verdict.md
  - lecture1_slides/05_p2_pitch.md
  - lecture1_slides/06_p2_verdict.md
  - lecture1_slides/07_p3_pitch.md
  - lecture1_slides/08_p3_verdict.md
  - lecture1_slides/09_p4_pitch.md
  - lecture1_slides/10_p4_verdict.md
  - lecture1_slides/11_p5_pitch.md
  - lecture1_slides/12_p5_verdict.md
  - lecture1_slides/13_p6_pitch.md
  - lecture1_slides/14_p6_verdict.md
  - lecture1_slides/15_pattern.md
  - lecture1_slides/16_takeaway.md
```





## The cost of getting them 
Poor requirements → perfect but unwanted software


- Functional vs. non-functional requirements with examples.


# References
1. Chapter 4, Requirements Engineering, Software Engineering, Ian Sommerville
