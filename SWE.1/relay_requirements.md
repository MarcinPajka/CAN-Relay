# Relay Requirements

## Document metadata
- Project: CAN-RELAY
- Document ID: 000001
- Version: 0.1
- Author: Marcin Pajka
- Date: 06.12.25
- Status: Draft

## Purpose
This document describes Relay module SWE 1 requirements.


## Requirement attribute template (per requirement)
| Name | Description |
|---|---|
| ID | Unique identifier (e.g., SYS-REQ-001) |
| Title | Short descriptive title |
| Description | Clear, singular behavior or constraint; testable |
| Acceptance / Success criteria | Measurable pass/fail criteria (what constitutes acceptance) |
| Verification method | Test / Analysis / Inspection / Demonstration / Review |
| Verification reference | Test-case ID, analysis report, review record, or evidence link |
| Allocation | System / Subsystem / Component where requirement is realized |
| Status | Proposed / Approved / Implemented / Verified / Closed |


## SWE.1 Requirements:

| Name | Description |
|---|---|
| ID                            | SYS-REQ-001 |
| Title                         | CAN interface |
| Description                   | Relay module shall use CAN interface to communicate with VCU module |
| Acceptance / Success criteria | CAN message exchange |
| Verification method           | Test  |
| Verification reference        |  --- |
| Allocation                    | Relay module |
| Status                        | Proposed  |

| Name | Description |
|---|---|
| ID                            | SYS-REQ-002 |
| Title                         | Ethernet interface |
| Description                   | Relay module shall use Ethernet interface to communicate with Client module (with JSON messages) |
| Acceptance / Success criteria | Ethernet message exchange |
| Verification method           | Test  |
| Verification reference        |  --- |
| Allocation                    | Relay module |
| Status                        | Proposed  |

| Name | Description |
|---|---|
| ID                            | SYS-REQ-003 |
| Title                         | CAN signal battery_level |
| Description                   | Relay module shall be able to receive CAN signal 'battery_level' containing percentage value of battery charge   |
| Acceptance / Success criteria | CAN message exchange |
| Verification method           | Test  |
| Verification reference        |  --- |
| Allocation                    | Relay module |
| Status                        | Proposed  |


| Name | Description |
|---|---|
| ID                            | SYS-REQ-004 |
| Title                         | Json message battery_level |
| Description                   | Relay module shall be able to send JSON message 'battery_level' containing percentage value of battery charge |
| Acceptance / Success criteria | JSON message exchange |
| Verification method           | Test  |
| Verification reference        |  --- |
| Allocation                    | Relay module |
| Status                        | Proposed  |


| Name | Description |
|---|---|
| ID                            | SYS-REQ-005 |
| Title                         | CAN signal velocity |
| Description                   | Relay module shall be able to receive CAN signal 'velocity' containing the current speed of the vehicle |
| Acceptance / Success criteria | CAN message exchange |
| Verification method           | Test  |
| Verification reference        |  --- |
| Allocation                    | Relay module |
| Status                        | Proposed  |

| Name | Description |
|---|---|
| ID                            | SYS-REQ-006 |
| Title                         | Json message velocity |
| Description                   | Relay module shall be able to send JSON message 'velocity' containing the current speed of the vehicle |
| Acceptance / Success criteria | JSON message exchange |
| Verification method           | Test |
| Verification reference        | --- |
| Allocation                    | Relay module |
| Status                        | Proposed |

| Name | Description |
|---|---|
| ID                            | SYS-REQ-007 |
| Title                         | CAN signal charging_active |
| Description                   | Relay module shall be able to receive CAN signal 'charging_active' containing the current charging status of the vehicle |
| Acceptance / Success criteria | CAN message exchange |
| Verification method           | Test  |
| Verification reference        |  --- |
| Allocation                    | Relay module |
| Status                        | Proposed  |


 Name | Description |
|---|---|
| ID                            | SYS-REQ-008 |
| Title                         | Json message charging_active |
| Description                   | Relay module shall be able to receive JSON message 'charging_active' containing the current charging status of the vehicle |
| Acceptance / Success criteria | JSON message exchange |
| Verification method           | Test |
| Verification reference        | --- |
| Allocation                    | Relay module |
| Status                        | Proposed |

| Name | Description |
|---|---|
| ID                            | SYS-REQ-009 |
| Title                         | CAN signal charge_request |
| Description                   | Relay module shall be able to receive CAN signal 'charge_request' containing the current charging status of the vehicle |
| Acceptance / Success criteria | CAN message exchange |
| Verification method           | Test  |
| Verification reference        |  --- |
| Allocation                    | Relay module |
| Status                        | Proposed  |

| Name | Description |
|---|---|
| ID                            | SYS-REQ-010 |
| Title                         | Json message charge_request |
| Description                   | Relay module shall be able to receive JSON message 'charge_request' containing the current charging status of the vehicle |
| Acceptance / Success criteria | JSON message exchange |
| Verification method           | Test |
| Verification reference        | --- |
| Allocation                    | Relay module |
| Status                        | Proposed |

| Name | Description |
|---|---|
| ID                            | SYS-REQ-011 |
| Title                         | Relay hardware |
| Description                   | Relay module shall run on Raspberry PI 5 |
| Acceptance / Success criteria | JSON message exchange |
| Verification method           | Test |
| Verification reference        | --- |
| Allocation                    | Relay module |
| Status                        | Proposed |


| Name | Description |
|---|---|
| ID                            | SYS-REQ-012 |
| Title                         | Relay CAN hardware |
| Description                   | Relay module shall use CAN extention HAT - MCP2515 + SI65HVD230 - Waveshare 17912 |
| Acceptance / Success criteria | JSON message exchange |
| Verification method           | Test |
| Verification reference        | --- |
| Allocation                    | Relay module |
| Status                        | Proposed |






## Requirements list (table)
| ID | Title     | Verification | Status |
|---|---|---|---|
| SYS-REQ-001 | CAN interface | Test | Proposed |
| SYS-REQ-002 | Ethernet interface | Test | Proposed |
| SYS-REQ-003 | CAN signal battery_level | Test | Proposed |
| SYS-REQ-004 | Json message battery_level | Test | Proposed |
| SYS-REQ-005 | CAN signal velocity | Test | Proposed |
| SYS-REQ-006 | Json message velocity | Test | Proposed |
| SYS-REQ-007 | CAN signal charging_active | Test | Proposed |
| SYS-REQ-008 | Json message charging_active | Test | Proposed |
| SYS-REQ-009 | CAN signal charge_request | Test | Proposed |
| SYS-REQ-010 | Json message charge_request | Test | Proposed |




## Verification & validation
- Reference V&V plan and test strategy.
- Map verification activities to requirements (unit/integration/system tests, reviews, analysis).
- Define entry/exit criteria for requirement verification.

## Change history
| Version | Date | Author | Summary |
|---|---|---|---|
| 0.1 | | | Initial template |

