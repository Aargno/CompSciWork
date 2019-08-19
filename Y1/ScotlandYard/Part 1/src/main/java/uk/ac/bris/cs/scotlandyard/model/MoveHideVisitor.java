/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package uk.ac.bris.cs.scotlandyard.model;

/**
 * Visitor class to simplify the hiding of moves as they are provided to notify
 * spectators.
 *
 * @author robey
 */
public class MoveHideVisitor implements MoveVisitor {

    private ScotlandYardModel model;

    /**
     * Constructor for the MoveHideVisitor
     *
     * @param model The model that the move is being made on
     */
    public MoveHideVisitor(ScotlandYardModel model) {
        this.model = model;
    }

    /**
     * Visit method for pass move, masks provided pass move before notifying spectators
     *
     * @param move The PassMove being passed to the visitor
     */
    @Override
    public void visit(PassMove move) {
        //Does nothing, as move is just passed
    }

    /**
     * Visit method for ticket move, masks provided ticket move before notifying spectators
     *
     * @param move Ticket move to be hidden
     */
    @Override
    public void visit(TicketMove move) {
        model.moveNotify(hideDestination(move, model.getPlayerLocation(move.colour()).orElse(-1)));
    }
    
    /**
     * Visit method for double move, masks provided double move before notifying spectators
     * 
     * @param move Double Move to be hidden
     */
    @Override
    public void visit(DoubleMove move) {
        handleDoubleMoveNotification(move);
    }

    /**
     * Handles the possible conditions for calling the onMoveMade notification
     * for doubleMove
     *
     * @param move The move used as a notification
     */
    private void handleDoubleMoveNotification(DoubleMove move) {
        DoubleMove tempMove;
        TicketMove t1 = hideDestination(move.firstMove(), model.getPlayerLocation(move.colour()).orElse(-1));
        TicketMove t2 = hideDestination(move.secondMove(), model.getPlayerLocation(move.colour()).orElse(-1));
        if (model.getRounds().get(model.getCurrentRound())) { //Check if reveal round will happen during intermediate move
            t1 = move.firstMove();
            t2 = hideDestination(move.secondMove(), t1.destination()); //Accounts for the new reveal location
        }
        if (model.getRounds().get(model.getCurrentRound() + 1)) {
            //last known location will be used as first move destination, defaults to -1 if player with move colour is somehow not in game
            t2 = move.secondMove();
        }
        tempMove = new DoubleMove(move.colour(), t1, t2);
        model.moveNotify(tempMove);
    }

    /**
     * Hides the destination of a given move, using a given mask
     *
     * @param move The move to have destination hidden
     * @param mask The shown destination
     * @return The altered ticket move with hidden destination
     */
    private TicketMove hideDestination(TicketMove move, int mask) {
        if (mask == -1) {
            throw new IllegalArgumentException("mask is -1, move colour can't be in game"); //Indicative of major error
        }
        return new TicketMove(move.colour(), move.ticket(), mask);
    }

}
