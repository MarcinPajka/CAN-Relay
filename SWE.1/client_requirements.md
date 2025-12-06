# Client Requirements

## Document metadata
- Project: 
- Document ID: 
- Version: 0.1
- Author: 
- Date: 
- Status: Draft / Review / Approved

## Purpose
State the purpose of this requirements document and its intended usage (system-level client requirements to be decomposed and traced).

## Scope
Define scope (system boundaries, included/excluded functionality, operating conditions).

## Normative references
List applicable standards, project-specifications, contracts, and ASPICE process descriptions.

## Terms & abbreviations
Define domain-specific terms and abbreviations (e.g., ASIL, HARA, VCU, ECU, SW, HW).

## System overview
Brief description of the system, block diagram reference, and major interfaces.

## Stakeholders & sources
- Client / Customer:
- Safety authority:
- Other stakeholders:
- Requirement sources (contract clause, use case ID, meeting, regulation): map each requirement to a source.

## Requirement ID format and rules
- ID scheme: [LEVEL]-[TYPE]-[NNN], e.g. SYS-REQ-001, SW-REQ-001, HWR-REQ-001
- Unique immutable ID per requirement.
- Requirements must be verifiable, unambiguous, complete, and testable.
- Each requirement must have owner, priority, verification method, and trace links.

## Requirement attribute template (per requirement)
- ID:
- Title:
- Type: Functional / Non-functional / Safety / Interface / Constraint / Performance / Derived / Design
- Priority: Must / Should / Could / Won't
- Source: contract/use-case/stakeholder/derived-from [ID]
- Description: (clear, singular behavior or constraint)
- Rationale: why required
- Acceptance / Success criteria: measurable pass/fail criteria
- Verification method: Test / Analysis / Inspection / Demonstration / Review
- Verification reference: test-case ID / analysis report
- Allocation: system/subsystem/component (where requirement is realized)
- Safety classification: ASIL A/B/C/D / SIL / Safety-critical (if applicable)
- Related requirements: upstream/downstream IDs
- Related design artifacts: architecture spec, interface spec, ICD
- Owner:
- Status: Proposed / Approved / Implemented / Verified / Closed
- Change history: date, author, change summary
- Comments / notes:

## Requirements list (table)
| ID | Title | Type | Priority | Source | Owner | Verification | Status |
|---|---:|---|---:|---|---|---|---|
| SYS-REQ-001 | | Functional | Must | | | Test | Proposed |

(Extend table with full attribute details in linked repository tool as needed.)

## Traceability matrix (high-level)
- Columns to include: Requirement ID, Derived from (higher-level ID), Allocated to (software/hardware/component), Design items, Test cases, Verification status, Closure date.

Example table:
| Req ID | Derived From | Allocated To | Design/Module | Test Case(s) | Verification Status |
|---|---|---|---|---|---|
| SYS-REQ-001 | CL-REQ-01 | ECU-FW | MotionControl | TC-001, TC-002 | Not verified |

## Safety & risk
- Reference HARA / FMEDA results.
- For safety-related requirements include ASIL, safety measures, fail-operational / fail-safe behavior, diagnostics requirements, monitoring.

## Verification & validation
- Reference V&V plan and test strategy.
- Map verification activities to requirements (unit/integration/system tests, reviews, analysis).
- Define entry/exit criteria for requirement verification.

## Configuration & change management
- Document controlled by configuration management system.
- Change control: require impact analysis, traceability update, review and approval for requirement changes.

## Review & approval
- Reviewers: names and roles
- Approval: name, role, date, signature (or electronic approval record)

## Change history
| Version | Date | Author | Summary |
|---|---|---|---|
| 0.1 | | | Initial template |

Notes:
- Keep each requirement atomic and testable.
- Ensure bidirectional traceability from customer needs down to tests and implementation.
- Maintain review logs and verification evidence per ASPICE expectations.
